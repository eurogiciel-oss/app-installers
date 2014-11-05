/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#pragma once

struct context
{
  /* id of the package to install */
  char *package_id;

  /* path of the package */
  char *package_path;

  /* path of the unpack directory */
  char *unpack_directory;

  /* path of the target directory */
  char *target_directory;

  /* path of the original package for drm step */
  char *package_path_drm_save;

#if 0
  DrmLicense *__pDrmLicense;
  InstallerError __error;
  InstallationStep __step;
  int __attributeType;
  InstallationState __state;
  int __stepChanged;
  int __continue;

  int __isOspCompat;
  int __isPreloaded;
  int __isHybridService;
  int __isVerificationMode;
  int __isUpdated;
  int __isAppSetting;
  int __isCsc;
  int __isUninstallable;

  InstallerOperation __operation;
  InstallationStorage __storage;
  RootCertificateType __rootCertType;
  int __privilegeLevel;
  int __packageNameType;
  int __apiVisibility;
  void *__pApp2ExtHandle;

  struct json_object *privileges;
  struct json_object *content;
    Tizen::Base::Collection::ArrayList * __pPrivilegeList;
    Tizen::Base::Collection::ArrayList * __pStringPrivilegeList;
    Tizen::Base::Collection::ArrayList * __pContentDataList;

    Tizen::Base::Collection::ArrayListT < char **>*__pAuthorCertList;
    Tizen::Base::Collection::ArrayListT < char **>*__pDistributorCertList;
    Tizen::Base::Collection::ArrayListT < char **>*__pDistributorCertList2;

  char *__coreXmlPath;
  char *__inputPath;
  char *__packagePath;
  char *__licensePath;
  char *__temporaryDir;
  char *__installDir;
  char *__workingDir;
    Tizen::App::PackageId __packageId;
  char *__version;
  char *__rootPath;
  char *__mainAppName;
  char *__apiVersion;
  char *__displayName;
  char *__privileges;
  char *__hmacPrivileges;
  char *__author;
  char *__url;
  char *__description;
    Tizen::App::PackageId __storeClientId;
  char *__additionalErrorString;
  char *__cscInfo;
  char *__virtualRootPath;

  char *__packageSize;

  int __certType;
  void *__pPkgmgrInstaller;
#endif
};
