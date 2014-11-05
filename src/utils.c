/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */
#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <limits.h>

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#ifdef NOTDEFINED
#include <dlfcn.h>

#include <unique_ptr.h>
#include <vconf.h>
#endif


int
utils_unzip (const char *targetdir, const char *zipfile)
{
  int status;
  char *command;

  /* prepare the command */
  status =
    asprintf (&command, "unzip -n -qq -d '%s' '%s'", targetdir, zipfile);
  if (status < 0)
    {
      errno = ENOMEM;
      return -1;
    }

  /* execute the command */
  status = system (command);
  free (command);

  /* diagnostic the command */
  switch (status)
    {
    case -1:
      syslog (LOG_ERR, "Can't excute unzip");
      break;
    case 0:
    case 1:
      return 0;
    case 4:
    case 5:
    case 6:
    case 7:
      errno = ENOMEM;
      break;
    case 9:
      syslog (LOG_ERR, "File to unzip doesn't exist: %s", zipfile);
      errno = ENOENT;
      break;
    default:
      errno = ECANCELED;
      syslog (LOG_ERR, "Error while unziping %s to %s", zipfile, targetdir);
    }
  return -1;
}




static int
_remove_directory_content (char path[PATH_MAX], int length)
{
  DIR *dir;
  struct dirent *ent;
  int n;

  if (length + 1 >= PATH_MAX)
    {
      errno = ENAMETOOLONG;
      return -1;
    }

  dir = opendir (path);
  if (!dir)
    return -1;

  path[length++] = '/';
  path[length] = 0;

  errno = 0;
  ent = readdir (dir);
  while (ent != NULL)
    {
      n = (int) strlen (ent->d_name);
      assert (n > 0);
      if (n > 2 || ent->d_name[0] != '.' || ent->d_name[n - 1] != '.')
	{
	  if (length + n >= PATH_MAX)
	    {
	      errno = ENAMETOOLONG;
	      break;
	    }
	  memcpy (path + length, ent->d_name, n + 1);
#ifdef _DIRENT_HAVE_D_TYPE
	  if (ent->d_type != DT_DIR)
	    {
	      if (unlink (path))
		break;
	    }
	  else
	    {
	      if (_remove_directory_content (path, length + n)
		  || rmdir (path))
		break;
	    }
#else
	  if (unlink (path) || errno != EISDIR
	      || _remove_directory_content (path, length + n) || rmdir (path))
	    break;
#endif
	}
      errno = 0;
      ent = readdir (dir);
    }
  closedir (dir);
  path[length - 1] = 0;
  return errno ? -1 : 0;
}

int
utils_remove_directory_content (const char *path)
{
  char buffer[PATH_MAX];
  int length;

  length = (int) strlen (path);
  if (length >= PATH_MAX)
    {
      errno = EINVAL;
      return -1;
    }
  memcpy (buffer, path, length + 1);
  return _remove_directory_content (buffer, length);
}

int
utils_remove_directory (const char *path, int force)
{
  if (force && utils_remove_directory_content (path))
    return -1;
  return rmdir (path);
}

int
utils_remove_any (const char *path)
{
  return unlink (path) || errno != EISDIR
    || utils_remove_directory (path, 1) ? -1 : 0;
}


int
utils_copy_file (const char *dest, const char *src, int force)
{
  int fdfrom, fdto, result;
  ssize_t length;
  size_t count;
  struct stat s;

  result = -1;
  fdfrom = open (src, O_RDONLY);
  if (fdfrom >= 0)
    {
      if (!fstat (fdfrom, &s))
	{
	  fdto = open (src, O_WRONLY | O_TRUNC | O_CREAT, s.st_mode | 0200);
	  if (fdto >= 0)
	    {
	      for (;;)
		{
		  count = (~(size_t) 0) >> 1;
		  if ((off_t) count > s.st_size)
		    count = (size_t) s.st_size;
		  length = sendfile (fdto, fdfrom, NULL, count);
		  if (length < 0)
		    break;
		  s.st_size -= (off_t) length;
		  if (s.st_size == 0)
		    {
		      result = 0;
		      break;
		    }
		}
	    }
	  close (fdto);
	  if (result)
	    unlink (dest);
	}
      close (fdfrom);
    }
  return result;
}


static int
_copy_directory (char dest[PATH_MAX], int destlen, char src[PATH_MAX],
		 int srclen, int force)
{
  DIR *dir;
  struct dirent *ent;
  int n;
#ifdef _DIRENT_HAVE_D_TYPE
  int sts;
#else
  struct stat st;
#endif

  if (srclen + 1 >= PATH_MAX || destlen + 1 > PATH_MAX)
    {
      errno = ENAMETOOLONG;
      return -1;
    }

  dir = opendir (src);
  if (!dir)
    return -1;

  src[srclen++] = '/';
  src[srclen] = 0;
  dest[destlen++] = '/';
  dest[destlen] = 0;

  errno = 0;
  ent = readdir (dir);
  while (ent != NULL)
    {
      n = (int) strlen (ent->d_name);
      assert (n > 0);
      if (n > 2 || ent->d_name[0] != '.' || ent->d_name[n - 1] != '.')
	{
	  if (srclen + n >= PATH_MAX || destlen + n >= PATH_MAX)
	    {
	      errno = ENAMETOOLONG;
	      break;
	    }
	  memcpy (src + srclen, ent->d_name, n + 1);
	  memcpy (dest + destlen, ent->d_name, n + 1);
#ifdef _DIRENT_HAVE_D_TYPE
	  switch (ent->d_type)
	    {
	    case DT_DIR:
	      sts = mkdir (dest, 0777);
	      if (!sts || errno == EEXIST)
		{
		  sts =
		    _copy_directory (dest, destlen + n, src, srclen + n,
				     force);
		}
	      break;
	    case DT_REG:
	      sts = utils_copy_file (dest, src, force);
	      break;
	    case DT_LNK:
	    default:
	      sts = 0;
	      break;
	    }
	  if (sts)
	    break;
#else
	  if (lstat (src, &st))
	    break;
	  if (S_ISDIR (st.st_mode))
	    {
	      if (mkdir (dest, 0777) && errno != EEXIST)
		break;
	      if (_copy_directory (dest, destlen + n, src, srclen + n, force))
		break;
	    }
	  else if (S_ISREG (st.st_mode))
	    {
	      if (utils_copy_file (dest, src, force))
		break;
	    }
#endif
	}
      errno = 0;
      ent = readdir (dir);
    }
  closedir (dir);
  src[srclen - 1] = 0;
  dest[destlen - 1] = 0;
  return errno ? -1 : 0;
}


int
utils_copy_directory (const char *dest, const char *src, int force)
{
  char bufdest[PATH_MAX], bufsrc[PATH_MAX];
  int lendest, lensrc;

  lendest = (int) strlen (dest);
  lensrc = (int) strlen (src);
  if (lendest >= PATH_MAX || lensrc >= PATH_MAX)
    {
      errno = EINVAL;
      return -1;
    }
  memcpy (bufdest, dest, lendest + 1);
  return _copy_directory (bufdest, lendest, bufsrc, lensrc, force);
}










#ifdef NOTDEFINED

bool
InstallerUtil::IsSymlink (const Tizen::Base::String & filePath)
{
  int err = -1;
  struct stat fileinfo;

  std::unique_ptr < char[] >
    pFilePath (_StringConverter::CopyToCharArrayN (filePath));
  TryReturn (pFilePath, false, "pFilePath is null");

  err = lstat (pFilePath.get (), &fileinfo);
  TryReturn (err >= 0, false, "lstat() failed(%s), file=[%s]",
	     strerror (errno), pFilePath.get ());

  if (S_ISLNK (fileinfo.st_mode))
    {
      return true;
    }

  return false;
}

bool
InstallerUtil::GetRealPath (const String & filePath, String & realPath)
{
  char *pRealPath = null;

  std::unique_ptr < char[] >
    pFilePath (_StringConverter::CopyToCharArrayN (filePath));
  TryReturn (pFilePath, false, "pFilePath is null");

  char tmpPath[PATH_MAX] = { 0 };
  pRealPath = realpath (pFilePath.get (), tmpPath);
  TryReturn (pRealPath, false, "pRealPath is null");

  realPath = tmpPath;

  AppLog ("GetRealPath(): path=[%ls], realPath=[%ls]", filePath.GetPointer (),
	  realPath.GetPointer ());

  return true;
}

bool
InstallerUtil::ChangeMode (const String & filePath, int mode)
{
  int err = -1;

  std::unique_ptr < char[] >
    pFilePath (_StringConverter::CopyToCharArrayN (filePath));
  TryReturn (pFilePath, false, "pFilePath is null");

  err = chmod (pFilePath.get (), mode);
  TryReturn (err == 0, false, "chmod() is failed(%s), file=[%s], mode=[%o]",
	     strerror (errno), pFilePath.get (), mode);

  return true;
}

bool
InstallerUtil::ChangeOwner (const String & filePath)
{
  int err = -1;

  std::unique_ptr < char[] >
    pFilePath (_StringConverter::CopyToCharArrayN (filePath));
  TryReturn (pFilePath, false, "pFilePath is null");

  err = chown (pFilePath.get (), APP_OWNER_ID, APP_GROUP_ID);
  TryReturn (err == 0, false, "chown() is failed(%s), file=[%s]",
	     strerror (errno), pFilePath.get ());

  return true;
}

bool
  InstallerUtil::ChangeDirectoryPermission (const String & file, int mode,
					    bool appOwner)
{
  result r = E_SUCCESS;
  bool res = false;

  res = File::IsFileExist (file);
  if (res == false)
    {
      AppLog ("path=[%ls]: skip", file.GetPointer ());
      return true;
    }

  std::unique_ptr < Directory > pDir (new (std::nothrow) Directory);
  TryReturn (pDir, false, "pDir is null.");

  r = pDir->Construct (file);
  TryReturn (!IsFailed (r), false, "pDir->Construct() failed, file=[%ls]",
	     file.GetPointer ());

  std::unique_ptr < DirEnumerator > pDirEnum (pDir->ReadN ());
  TryReturn (pDirEnum, false, "pDirEnum is null.");

  while (pDirEnum->MoveNext () == E_SUCCESS)
    {
      DirEntry entry = pDirEnum->GetCurrentDirEntry ();
      String entryName = entry.GetName ();
      if (entryName.IsEmpty () == true)
	{
	  AppLog ("entryName is empty.", entryName.GetPointer ());
	  continue;
	}

      String entryDir = file;
      entryDir += L"/";
      entryDir += entryName;

      if (entryName == L".")
	{
	  if (appOwner == true)
	    {
	      InstallerUtil::ChangeOwner (entryDir);
	    }
	  InstallerUtil::ChangeMode (entryDir, mode | PERM_EXECUTE);
	  continue;
	}
      else if (entryName == L"..")
	{
	  continue;
	}

      if (entry.IsDirectory () == false)
	{
	  if (appOwner == true)
	    {
	      InstallerUtil::ChangeOwner (entryDir);
	    }
	  InstallerUtil::ChangeMode (entryDir, mode);
	}
      else
	{
	  ChangeDirectoryPermission (entryDir, mode, appOwner);
	  if (appOwner == true)
	    {
	      InstallerUtil::ChangeOwner (entryDir);
	    }
	  InstallerUtil::ChangeMode (entryDir, mode | PERM_EXECUTE);
	}
    }

  AppLog ("path=[%ls], mode=[%04o], appOwner=[%s]",
	  file.GetPointer (), mode, appOwner ? "true" : "false");

  return true;
}

String
InstallerUtil::GetCategory (int categoryType)
{
  String category;

  if (categoryType == CATEGORY_TYPE_IME)
    {
      category = L"Ime";
    }
  else if (categoryType == CATEGORY_TYPE_HOME_SCREEN)
    {
      category = L"home-screen";
    }
  else if (categoryType == CATEGORY_TYPE_LOCK_SCREEN)
    {
      category = L"lock-screen";
    }

  return category;
}

int
InstallerUtil::GetCategoryType (char *pCategory)
{
  CategoryType category = CATEGORY_TYPE_NONE;

  if (strcasecmp (pCategory, "Ime") == 0)
    {
      category = CATEGORY_TYPE_IME;
    }
  else if (strcasecmp (pCategory, "home-screen") == 0)
    {
      category = CATEGORY_TYPE_HOME_SCREEN;
    }
  else if (strcasecmp (pCategory, "lock-screen") == 0)
    {
      category = CATEGORY_TYPE_LOCK_SCREEN;
    }

  return category;
}

bool
  InstallerUtil::CreateSymlinkForAppDirectory (const String & inPath,
					       String & outPath)
{
  String appId;

  int length = inPath.GetLength ();
  inPath.SubString (length - PACKAGE_ID_LENGTH, PACKAGE_ID_LENGTH, appId);

  String newPath;
  newPath = PATH_OPT_APPS;
  newPath += L"/";
  newPath += appId;

  if (inPath != newPath)
    {
      InstallerUtil::CreateSymlink (inPath, newPath);
    }

  outPath = newPath;
  AppLog ("CreateSymlinkForAppDirectory(): output path=[%ls]",
	  outPath.GetPointer ());

  return true;
}

bool
  InstallerUtil::CreateInfoFile (const String & filePath,
				 const String * pContext)
{
  result r = E_SUCCESS;
  File file;

  r = file.Construct (filePath, "w");
  TryReturn (!IsFailed (r), false, "file.Construct() failed, filePath=[%ls]",
	     filePath.GetPointer ());

  AppLog ("------------------------------------------");
  AppLog ("CreateInfoFile(), filePath = [%ls]", filePath.GetPointer ());

  if (pContext)
    {
      r = file.Write (*pContext);
      TryReturn (!IsFailed (r), false, "file.Write() failed, filePath=[%ls]",
		 filePath.GetPointer ());
      AppLog ("string = [%ls]", pContext->GetPointer ());
    }
  AppLog ("------------------------------------------");

  return true;
}

bool
InstallerUtil::CreateFlagFile (const String & filePath)
{
  bool res = true;
  int fd = -1;
  FILE *pFile = null;
  File file;

  std::unique_ptr < char[] >
    pFilePath (_StringConverter::CopyToCharArrayN (filePath));
  TryCatch (pFilePath, res = false, "pFilePath is null.");

  pFile = fopen (pFilePath.get (), "w");
  TryCatch (pFile, res = false, "fopen(%s, w) failed.", pFilePath.get ());

  fd = fileno (pFile);
  TryCatch (fd != -1, res = false, "fileno(%s) failed.", pFilePath.get ());

  fsync (fd);
  AppLog ("CreateFlagFile(), filePath = [%ls]", filePath.GetPointer ());

CATCH:
  if (pFile)
    {
      fclose (pFile);
    }

  return res;
}

bool
  InstallerUtil::GetRdsList (const PackageId & packageId,
			     IList * pDeletedList, IList * pAddedList,
			     IList * pModifiedList)
{
  bool res = true;
  FILE *fp = null;
  char rdsFilePath[1024] = { 0 };
  char buffer[1024] = { 0 };
  InstallerRdsState state = INSTALLER_RDS_STATE_NONE;

  snprintf (rdsFilePath, sizeof (rdsFilePath), "%s/%ls/%s",
	    DIR_APPLICATIONS_TMP, packageId.GetPointer (),
	    INSTALLER_RDS_FILE_NAME);

  fp = fopen (rdsFilePath, "r");
  TryReturn (fp, false, "fp is null.");
  AppLog (".rds_delta file");
  int line = 1;

  while (fgets (buffer, sizeof (buffer), fp) != null)
    {
      bool isMetadata = false;

      if (buffer[0] == '#')
	{
	  if (strcasestr (buffer, INSTALLER_RDS_DELETE_STR))
	    {
	      state = INSTALLER_RDS_STATE_DELETE;
	    }
	  else if (strcasestr (buffer, INSTALLER_RDS_ADD_STR))
	    {
	      state = INSTALLER_RDS_STATE_ADD;
	    }
	  else if (strcasestr (buffer, INSTALLER_RDS_MODIFY_STR))
	    {
	      state = INSTALLER_RDS_STATE_MODIFY;
	    }

	  isMetadata = true;
	}

      if (state == INSTALLER_RDS_STATE_NONE)
	{
	  AppLog ("Unknown RDS State, INSTALLER_RDS_STATE_NONE");
	  continue;
	}

      std::unique_ptr < String > pStr (new (std::nothrow) String (buffer));
      TryCatch (pStr, res = false, "pStr is null.");
      TryCatch (pStr->IsEmpty () == false, res = false, "pStr is empty.");

      pStr->Trim ();
      AppLog (".rds_delta: line(%03d)=[%ls]", line, pStr->GetPointer ());
      line++;

      if (isMetadata == true)
	continue;

      if (state == INSTALLER_RDS_STATE_DELETE)
	{
	  pDeletedList->Add (pStr.release ());
	}
      else if (state == INSTALLER_RDS_STATE_ADD)
	{
	  pAddedList->Add (pStr.release ());
	}
      else if (state == INSTALLER_RDS_STATE_MODIFY)
	{
	  pModifiedList->Add (pStr.release ());
	}

      memset (buffer, 0, sizeof (buffer));
    }

CATCH:
  fclose (fp);
  return res;
}

const char *
InstallerUtil::GetInstallerOperationString (int operation)
{
  if (operation == INSTALLER_OPERATION_INSTALL)
    {
      return "Install";
    }
  else if (operation == INSTALLER_OPERATION_UNINSTALL)
    {
      return "Uninstall";
    }
  else if (operation == INSTALLER_OPERATION_REINSTALL)
    {
      return "Reinstall";
    }

  return "Unknown";
}

bool
  InstallerUtil::GetFileDigest (const String & filePath, String & digestValue)
{
  const int bufSize = 64 * 1024;
  int readBytes = 0;
  result r = E_SUCCESS;

  File file;
  std::unique_ptr < Sha2Hash > pHash (new (std::nothrow) Sha2Hash ());

  r = pHash->SetAlgorithm ("SHA2/256");
  TryReturn (!IsFailed (r), false, "pHash->SetAlgorithm() is failed.");

  r = pHash->Initialize ();
  TryReturn (!IsFailed (r), false, "pHash->Initialize() is failed.");

  std::unique_ptr < char[] > pBuf (new (std::nothrow) char[bufSize]);
  TryReturn (pBuf, false, "pBuf is null");

  r = file.Construct (filePath, L"r");
  TryReturn (!IsFailed (r), false, "file.Construct() is failed.");

  do
    {
      readBytes = file.Read (pBuf.get (), bufSize);
      AppLog ("readBytes for Hash=[%d]", readBytes);

      if (readBytes > 0)
	{
	  ByteBuffer buffer;
	  r =
	    buffer.Construct ((const byte *) pBuf.get (), 0, readBytes,
			      bufSize);
	  TryReturn (!IsFailed (r), false, "buffer.Construct() is failed.");

	  r = pHash->Update (buffer);
	  TryReturn (!IsFailed (r), false, "pHash->Update() is failed.");
	}
    }
  while (readBytes > 0);

  std::unique_ptr < ByteBuffer > pResultBuf (pHash->FinalizeN ());
  TryReturn (pResultBuf, false, "pResultBuf is null.");

  r = StringUtil::EncodeToBase64String (*pResultBuf, digestValue);
  TryReturn (!IsFailed (r), false, "EncodeToBase64String() is failed.");

  return true;
}

IMap *
InstallerUtil::ParseN (const String & str, const String & tokenDelimiter)
{
  TryReturn (str.IsEmpty () == false, null, "str is empty.");
  TryReturn (tokenDelimiter.IsEmpty () == false, null,
	     "tokenDelimiter is empty.");

  std::unique_ptr < HashMap > pMap (new (std::nothrow) HashMap);
  TryReturn (pMap, null, "pMap is null.");

  result r = pMap->Construct ();
  TryReturn (!IsFailed (r), null, "pMap->Construct() is failed.");

  StringTokenizer strTok (str, tokenDelimiter);
  while (strTok.HasMoreTokens () == true)
    {
      String token;
      r = strTok.GetNextToken (token);
      TryReturn (!IsFailed (r), null, "strTok.GetNextToken() is failed.");

      AppLog ("token = [%ls]", token.GetPointer ());

      StringTokenizer infoTok (token, L"=");

      if (infoTok.GetTokenCount () != 2)
	{
	  AppLog ("'=' is not existed.");
	  continue;
	}

      std::unique_ptr < String > pKey (new (std::nothrow) String);
      r = infoTok.GetNextToken (*pKey);
      TryReturn (!IsFailed (r), null,
		 "infoTok.GetNextToken(*pKey) is failed.");
      AppLog (" - key = [%ls]", pKey->GetPointer ());

      std::unique_ptr < String > pValue (new (std::nothrow) String);
      r = infoTok.GetNextToken (*pValue);
      TryReturn (!IsFailed (r), null,
		 "infoTok.GetNextToken(*pValue) is failed.");
      AppLog (" - value = [%ls]", pValue->GetPointer ());

      r = pMap->Add (pKey.release (), pValue.release ());
      TryReturn (!IsFailed (r), null, "pMap->Add() is failed.");
    }

  if (pMap->GetCount () <= 0)
    {
      AppLog ("pMap->GetCount() is invalid.");
      return null;
    }

  return pMap.release ();
}

bool
InstallerUtil::TerminateApp (const AppId & appId)
{
  bool res = true;

  if (_Aul::IsRunning (appId) == true)
    {
      AppLog ("App(%ls) is running.", appId.GetPointer ());

      result r = _Aul::TerminateApplication (appId);
      TryReturn (r == E_SUCCESS, false,
		 "TerminateApplication() failed. [%ls]", appId.GetPointer ());

      for (int j = 0; j < TERMINATE_RETRY_COUNT; j++)
	{
	  res = _Aul::IsRunning (appId);
	  if (res == false)
	    {
	      AppLog ("App(%ls) is terminated.", appId.GetPointer ());
	      break;
	    }
	  else
	    {
	      AppLog ("App(%ls) is not terminated yet. wait count = [%d]",
		      appId.GetPointer (), j);
	      usleep (100000);
	    }
	}

      if (res == true)
	{
	  AppLog ("App(%ls) can't be terminated.", appId.GetPointer ());
	  return false;
	}
    }
  else
    {
      AppLog ("App(%ls) is not running.", appId.GetPointer ());
    }

  return true;
}

bool
  InstallerUtil::TerminateApps (const PackageId & packageId,
				bool DisableAutoRestart)
{
  std::unique_ptr < PackageInfo >
    pPackageInfo (_PackageManagerImpl::
		  GetInstance ()->GetPackageInfoN (packageId));
  TryReturn (pPackageInfo, false, "GetPackageInfoN() failed.");

  _PackageInfoImpl *pPackageInfoImpl =
    _PackageInfoImpl::GetInstance (pPackageInfo.get ());
  TryReturn (pPackageInfoImpl, false, "GetInstance() failed.");

  std::unique_ptr < IList >
    pPackageAppList (pPackageInfoImpl->GetPackageAppInfoListN ());
  TryReturn (pPackageAppList, false, "GetPackageAppInfoListN() failed.");

  for (int i = 0; i < pPackageAppList->GetCount (); i++)
    {
      PackageAppInfo *pPackageAppInfo =
	dynamic_cast < PackageAppInfo * >(pPackageAppList->GetAt (i));
      TryReturn (pPackageAppInfo, false, "pPackageAppList->GetAt(%d) failed.",
		 i);

      AppId appId = pPackageAppInfo->GetAppId ();

      if (DisableAutoRestart == true)
	{
	  _PackageAppInfoImpl *pAppInfoImpl =
	    _PackageAppInfoImpl::GetInstance (pPackageAppInfo);
	  TryReturn (pAppInfoImpl, false, "pAppInfoImpl is null.");

	  String key ("AutoRestart");
	  if (pAppInfoImpl->GetAppFeature (key) == L"True")
	    {
	      AppLog ("AutoRestart = [True]");

	      DatabaseManager databaseManager;
	      databaseManager.DisableAutoRestart (appId);

	      AppLog ("AutoRestart[%ls] is updated.",
		      pAppInfoImpl->GetAppFeature (key).GetPointer ());
	    }
	}

      TerminateApp (appId);
    }

  return true;
}

bool
InstallerUtil::IsUninstallable (const PackageId & packageId)
{
  bool res = false;

  std::unique_ptr < PackageInfo >
    pPackageInfo (_PackageManagerImpl::
		  GetInstance ()->GetPackageInfoN (packageId));
  TryReturn (pPackageInfo, false, "GetPackageInfoN() failed.");

  _PackageInfoImpl *pPackageInfoImpl =
    _PackageInfoImpl::GetInstance (pPackageInfo.get ());
  TryReturn (pPackageInfoImpl, false, "GetInstance() failed.");

  res = pPackageInfoImpl->IsUninstallable ();

  AppLog ("packageId[%ls]: Uninstallable = [%s]", packageId.GetPointer (),
	  res ? "true" : "false");

  return res;
}

bool
  InstallerUtil::IsCscPackage (const PackageId & packageId, String & cscInfo)
{
  bool res = false;
  int result = 0;
  char *pPath = null;
  pkgmgrinfo_pkginfo_h handle = null;

  std::unique_ptr < char[] >
    pPackageId (_StringConverter::CopyToCharArrayN (packageId));
  TryReturn (pPackageId, false, "pPackageId is null.");

  result = pkgmgrinfo_pkginfo_get_pkginfo (pPackageId.get (), &handle);
  TryReturn (result == PMINFO_R_OK, false,
	     "pkgmgrinfo_pkginfo_get_pkginfo() failed. result=[%d], package=[%s]",
	     result, pPackageId.get ());

  result = pkgmgrinfo_pkginfo_get_csc_path (handle, &pPath);
  TryReturn (result == PMINFO_R_OK, false,
	     "pkgmgrinfo_pkginfo_get_csc_path() failed. result=[%d], package=[%s]",
	     result, pPackageId.get ());

  AppLog ("csc_path = [%s]", pPath);

  cscInfo = pPath;

  if (cscInfo.IsEmpty () == false)
    {
      res = true;
      AppLog ("packageId[%ls]: cscInfo = [%ls]", packageId.GetPointer (),
	      cscInfo.GetPointer ());
    }

  if (handle)
    {
      pkgmgrinfo_pkginfo_destroy_pkginfo (handle);
    }

  return res;
}

bool
InstallerUtil::IsDefaultExternalStorage ()
{
  int res = 0;
  int storage = 0;
  int mmcStatus = VCONFKEY_SYSMAN_MMC_REMOVED;

  res =
    vconf_get_int ("db/setting/default_memory/download_application",
		   &storage);
  TryReturn (res == 0, false,
	     "vconf_get_int(db/setting/default_memory/download_application) failed.");

  AppLog ("Storage = [%d]", storage);

  if (storage == 1)
    {
      res = vconf_get_int (VCONFKEY_SYSMAN_MMC_STATUS, &mmcStatus);
      TryReturn (res == 0, false,
		 "vconf_get_int(VCONFKEY_SYSMAN_MMC_STATUS) failed.");

      if ((mmcStatus == VCONFKEY_SYSMAN_MMC_REMOVED)
	  || (mmcStatus == VCONFKEY_SYSMAN_MMC_INSERTED_NOT_MOUNTED))
	{
	  AppLog ("mmcStatus is MMC_REMOVED or NOT_MOUNTED.");
	}
      else
	{
	  AppLog ("mmcStatus is MMC_MOUNTED.");
	  return true;
	}
    }

  return false;
}

bool
InstallerUtil::IsSignatureVerificationEnabled ()
{
  result r;
  Registry reg;
  String section (L"feature");
  String entry (L"signature");
  String value;

  r = reg.Construct (CONFIG_PATH, "r");
  TryReturn (!IsFailed (r), false, "CONFIG file is not found.");

  r = reg.GetValue (section, entry, value);
  TryReturn (!IsFailed (r), false, "GetValue is failed. entry = [%ls]",
	     entry.GetPointer ());

  AppLog ("[%ls is %ls.]", entry.GetPointer (), value.GetPointer ());

  if (value == L"on")
    {
      return true;
    }

  return false;
}

bool
InstallerUtil::IsAuthorSignatureVerificationEnabled ()
{
  result r;
  Registry reg;
  String section (L"feature");
  String entry (L"author-signature");
  String value;

  r = reg.Construct (CONFIG_PATH, "r");
  TryReturn (!IsFailed (r), false, "CONFIG file is not found.");

  r = reg.GetValue (section, entry, value);
  TryReturn (!IsFailed (r), false, "GetValue is failed. entry = [%ls]",
	     entry.GetPointer ());

  AppLog ("[%ls is %ls.]", entry.GetPointer (), value.GetPointer ());

  if (value == L"on")
    {
      return true;
    }

  return false;
}

long long
InstallerUtil::GetDirectorySize (const String & rootPath)
{
  long long total = 0;
  long long ret = 0;
  int q = 0;
  int r = 0;
  DIR *dp = NULL;
  struct dirent *ep = NULL;
  struct stat fileinfo;
  char file[FILENAME_MAX] = { 0, };
  char *pDirName = null;

  pDirName = _StringConverter::CopyToCharArrayN (rootPath);
  TryCatch (pDirName, total = 0, "[osp-installer][libtpk] pDirName is null");

  dp = opendir (pDirName);
  TryCatch (dp, total = 0, "[osp-installer][libtpk] dp is null");

  while ((ep = readdir (dp)) != NULL)
    {
      if (!strcmp (ep->d_name, ".") || !strcmp (ep->d_name, ".."))
	{
	  continue;
	}

      snprintf (file, FILENAME_MAX, "%s/%s", pDirName, ep->d_name);

      if (lstat (file, &fileinfo) < 0)
	{
	  continue;
	}

      if (S_ISLNK (fileinfo.st_mode))
	{
	  AppLog ("SYMLINK=%s", file);
	  total += INSTALLER_BLOCK_SIZE;
	  continue;
	}
      else if (S_ISDIR (fileinfo.st_mode))
	{
	  ret = GetDirectorySize (file);
	  ret += fileinfo.st_size;
	  total += ret;
	  AppLog ("DIR=%s, size=%d[%d KB],", file, (int) ret,
		  (int) (ret / 1024));
	}
      else
	{
	  /*It is a file. Calculate the actual size occupied (in terms of 4096 blocks) */
	  q = (fileinfo.st_size / INSTALLER_BLOCK_SIZE);
	  r = (fileinfo.st_size % INSTALLER_BLOCK_SIZE);
	  if (r)
	    {
	      q++;
	    }
	  total += q * INSTALLER_BLOCK_SIZE;

	  if (q)
	    {
	      // AppLog("File=%s, size=%d[%d KB]", file, (q * INSTALLER_BLOCK_SIZE), (q * INSTALLER_BLOCK_SIZE)/1024);
	    }
	}
    }

  closedir (dp);
  dp = null;

CATCH:
  delete[]pDirName;

  return total;
}

String
InstallerUtil::GetDisplaySize (const Tizen::Base::String & path)
{
  long long int size = 0;
  float sizeRes = 0.0;
  String unitStr = L"0 B";
  result r = E_SUCCESS;

  FileAttributes attr;
  r = File::GetAttributes (path, attr);
  TryReturn (!IsFailed (r), L"0.0KB", "File::GetAttributes() failed");

  if (attr.IsDirectory ())
    {
      size = GetDirectorySize (path);

      // root directory
      size += INSTALLER_BLOCK_SIZE;
      AppLog ("DIR=%ls, size=%d[%dKB],", path.GetPointer (), (int) size,
	      (int) (size / 1024));
    }
  else
    {
      size = attr.GetFileSize ();
    }

  sizeRes = size / 1024.0;

  String resStr;
  resStr.Format (128, L"%.1f", sizeRes);

  int strLen = resStr.GetLength ();

  for (int i = strLen - 5; i > 0; i = i - 3)
    {
      resStr.Insert (L",", i);
    }

  resStr += L"KB";

  return resStr;
}

Tizen::Base::_ApiVisibility
  InstallerUtil::ConvertToNativeApiVisibility (int apiVisibility)
{
  switch (apiVisibility)
    {
    case API_VISIBILITY_PUBLIC:
      return Tizen::Base::_API_VISIBILITY_PUBLIC;

    case API_VISIBILITY_PARTNER:
      return Tizen::Base::_API_VISIBILITY_PARTNER;

    case API_VISIBILITY_PLATFORM:
      return Tizen::Base::_API_VISIBILITY_PLATFORM;

    default:
      return Tizen::Base::_API_VISIBILITY_NONE;
    }
}

int
InstallerUtil::GetPrivilegeLevel (int apiVisibility)
{
  if (apiVisibility == _API_VISIBILITY_PLATFORM)
    {
      return PRIVILEGE_LEVEL_PLATFORM;
    }
  else if (apiVisibility == _API_VISIBILITY_PARTNER)
    {
      return PRIVILEGE_LEVEL_PARTNER;
    }

  return PRIVILEGE_LEVEL_PUBLIC;
}

#endif
