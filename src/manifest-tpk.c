/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

#include "fail.h"
#include "manifest-tpk.h"
#include "xml-read.h"

/* facility macros for handling XML */

#define BEG($id) \
  static int beg_##$id(struct xml_reader*reader,const char*name,const char**attrs,struct manifest_tpk*manif)

#define END($id) \
  static int end_##$id(struct xml_reader*reader,const char*name,struct manifest_tpk*manif)

#define CHA($id) \
  static int cha_##$id(struct xml_reader*reader,const char*ch,int len,struct manifest_tpk*manif)

#define _XRE($id,$tag,$begin,$end,$characters) \
  static DECL_XML_READ_ELEM(xre_##$id,#$tag,$begin,$end,$characters)

#define _XRE_B($id,$tag) BEG($id); XRE($id,$tag,beg_##$id,0,0)
#define _XRE_E($id,$tag) END($id); XRE($id,$tag,0,end_##$id,0)
#define _XRE_C($id,$tag) CHA($id); XRE($id,$tag,0,0,cha_##$id)
#define _XRE_BE($id,$tag) BEG($id); END($id); XRE($id,$tag,beg_##$id,end_##$id,0)
#define _XRE_BC($id,$tag) BEG($id); CHA($id); XRE($id,$tag,beg_##$id,0,cha_##$id)
#define _XRE_EC($id,$tag) END($id); CHA($id); XRE($id,$tag,0,end_##$id,cha_##$id)
#define _XRE_BEC($id,$tag) BEG($id); END($id); CHA($id); XRE($id,$tag,beg_##$id,end_##$id,cha_##$id)

#define XRE($id,$begin,$end,$characters) _XRE($id,$id,$begin,$end,$characters)

#define XRE_B($id) BEG($id); XRE($id,beg_##$id,0,0)
#define XRE_E($id) END($id); XRE($id,0,end_##$id,0)
#define XRE_C($id) CHA($id); XRE($id,0,0,cha_##$id)
#define XRE_BE($id) BEG($id); END($id); XRE($id,beg_##$id,end_##$id,0)
#define XRE_BC($id) BEG($id); CHA($id); XRE($id,beg_##$id,0,cha_##$id)
#define XRE_EC($id) END($id); CHA($id); XRE($id,0,end_##$id,cha_##$id)
#define XRE_BEC($id) BEG($id); END($id); CHA($id); XRE($id,beg_##$id,end_##$id,cha_##$id)

END (simple_pop);
#define XRE_B_($id) BEG($id); XRE($id,beg_##$id,end_simple_pop,0)

/*------------------------------------------------------------------------------------------------------------
SEE
https://developer.tizen.org/dev-guide/2.2.1/org.tizen.native.appprogramming/html/ide_sdk_tools/manifest_element_hierarchy.htm

<Manifest>
|-<Id>
|-<Version>
|-<InstallationLocation>
|-<Type>
|-<Author>
|-<Url>
|-<Descriptions>
| `-<Description>
|-<Requirements>
| `-<Feature>
`-<Apps>
  |-<ApiVersion>
  |-<Privileges>
  | `-<Privilege>
  |-<UiApp>
  | |-<DisplayNames>
  | | `-<DisplayName>
  | |-<Icons>
  | | `-<Icon>
  | |-<AppControls>
  | | `-<AppControl>
  | |   `-<Capability>
  | |     `-<Resolution>
  | |-<LaunchConditions>
  | | `-<Condition>
  | |-<Notifications>
  | | `-<Notification>
  | |-<UiScalability>
  | |-<UiTheme>
  | |-<Ime>
  | | |-<Uuid>
  | | `-<Languages>
  | |   `-<Language>
  | `-<Accounts>
  |   `-<AccountProvider>
  |     |-<Icons>
  |     | `-<Icon>
  |     |-<DisplayNames>
  |     | `-<DisplayName>
  |     `-<Capabilities>
  |       `-<Capability>
  `-<ServiceApp>
    |-<DisplayNames>
    | `-<DisplayName>
    |-<Icons>
    | `-<Icon>
    |-<AppControls>
    | `-<AppControl>
    |   `-<Capability>
    |     `-<Resolution>
    |-<LaunchConditions>
    | `-<Condition>
    |-<Notifications>
    | `-<Notification>
    |-<DataControls>
    | `-<DataControl>
    |   `-<DataControlType>
    |-<AppWidgets>
      `-<AppWidget>
        |-<DisplayNames>
        | `-<DisplayName>
        |-<Sizes>
        | `-<Size>
        `-<ConfigurationAppControlAppId>
*/


/*
`-<Apps>
  |-<UiApp>

  | `-<Accounts>
  |   `-<AccountProvider>
  |     |-<Icons>
  |     | `-<Icon>
  |     |-<DisplayNames>
  |     | `-<DisplayName>
  |     `-<Capabilities>
  |       `-<Capability>
  `-<ServiceApp>

    |-<AppWidgets>
      `-<AppWidget>
        |-<DisplayNames>
        | `-<DisplayName>
        |-<Sizes>
        | `-<Size>
        `-<ConfigurationAppControlAppId>
*/

XRE_BE (Manifest);

XRE_C (Id);
XRE_C (Version);
XRE_C (InstallationLocation);
XRE_C (Type);
XRE_C (Author);
XRE_C (Url);

XRE_B_ (Descriptions);
XRE_BC (Description);

XRE_B_ (Requirements);
XRE_BC (Feature);

XRE_B_ (Apps);

XRE_C (ApiVersion);
XRE_B_ (Privileges);
XRE_C (Privilege);

XRE_B_ (DisplayNames);
XRE_BC (DisplayName);
XRE_B_ (Icons);
XRE_BC (Icon);
XRE_B_ (LaunchConditions);
XRE_BC (Condition);
XRE_B_ (Notifications);
XRE_BC (Notification);
XRE_B_ (AppControls);
XRE_B_ (AppControl);
XRE_B_ (Capability);
XRE_B (Resolution);

XRE_B_ (UiApp);

XRE_B_ (ServiceApp);

XRE_B_ (DataControls);
XRE_B_ (DataControl);
XRE_BC (DataControlType);

XRE_B (UiScalability);
XRE_B (UiTheme);

XRE_B_ (Ime);
XRE_C (Uuid);
XRE_B_ (Languages);
XRE_C (Language);

static DECL_XML_READ_ELEM (xre_any_ok, "", 0, 0, 0);



/*================== DUMPING =====================*/

#if !defined(NDEBUG)
#include <stdio.h>
#define nn(x) ((x) ? x : "NULL")
void
manifest_tpk_dump (struct manifest_tpk *manif)
{
  int i, j, k, l;
  printf ("Manifest (locale=%s)\n", nn (manif->locale));
  printf ("  Id.................. %s\n", nn (manif->id));
  printf ("  Version............. %s\n", nn (manif->version));
  printf ("  InstallationLocation %s\n", nn (manif->installation_location));
  printf ("  Type................ %s\n", nn (manif->type));
  printf ("  Author.............. %s\n", nn (manif->author));
  printf ("  Url................. %s\n", nn (manif->url));
  printf ("  Descriptions:\n");
  for (i = 0; i < manif->descriptions.count; i++)
    printf ("    %s: %s\n", nn (manif->descriptions.descriptions[i].locale),
	    nn (manif->descriptions.descriptions[i].value));
  printf ("  Requirements:\n");
  for (i = 0; i < manif->features.count; i++)
    printf ("    %s: %s\n", nn (manif->features.features[i].name),
	    nn (manif->features.features[i].value));
  printf ("  APPS:\n");
  printf ("    api_version............... %s\n", nn (manif->api_version));
  printf ("    privileges:\n");
  for (j = 0; j < manif->privileges.count; j++)
    printf ("      ... %s\n", nn (manif->privileges.privileges[j]));
  for (i = 0; i < manif->apps.count; i++)
    {
      printf ("    APP %d:\n", i);
      printf ("      name...................... %s\n",
	      nn (manif->apps.apps[i].name));
      printf ("      type...................... %d\n",
	      manif->apps.apps[i].type);
      printf ("      main...................... %s\n",
	      nn (manif->apps.apps[i].main));
      printf ("      menu_icon_visible......... %s\n",
	      nn (manif->apps.apps[i].menu_icon_visible));
      printf ("      launching_history_visible. %s\n",
	      nn (manif->apps.apps[i].launching_history_visible));
      printf ("      category.................. %s\n",
	      nn (manif->apps.apps[i].category));
      printf ("      hw_acceleration........... %s\n",
	      nn (manif->apps.apps[i].hw_acceleration));
      printf ("      sub_mode.................. %s\n",
	      nn (manif->apps.apps[i].sub_mode));
      printf ("      launch_on_boot............ %s\n",
	      nn (manif->apps.apps[i].launch_on_boot));
      printf ("      auto_restart.............. %s\n",
	      nn (manif->apps.apps[i].auto_restart));
      printf ("      use_ui.................... %s\n",
	      nn (manif->apps.apps[i].use_ui));
      printf ("      name...................... %s\n",
	      nn (manif->apps.apps[i].name));
      printf ("      name...................... %s\n",
	      nn (manif->apps.apps[i].name));

      printf ("      coordinate system......... %s\n",
	      nn (manif->apps.apps[i].ui_scalability.coordinate_system));
      printf ("      base screen size.......... %s\n",
	      nn (manif->apps.apps[i].ui_scalability.base_screen_size));
      printf ("      logical coordinate........ %s\n",
	      nn (manif->apps.apps[i].ui_scalability.logical_coordinate));

      printf ("      system theme.............. %s\n",
	      nn (manif->apps.apps[i].ui_theme.system_theme));
      printf ("      user defined theme........ %s\n",
	      nn (manif->apps.apps[i].ui_theme.user_defined_theme));

      printf ("      ime uuid.................. %s\n",
	      nn (manif->apps.apps[i].ime.uuid));
      printf ("      ime languages.............");
      for (j = 0; j < manif->apps.apps[i].ime.languages.count; j++)
	printf (" %s", nn (manif->apps.apps[i].ime.languages.languages[j]));
      printf ("\n");

      printf ("      display names:\n");
      for (j = 0; j < manif->apps.apps[i].display_names.count; j++)
	printf ("        (%s) %s\n",
		nn (manif->apps.apps[i].display_names.
		    display_names[j].locale),
		nn (manif->apps.apps[i].display_names.
		    display_names[j].value));
      printf ("      icons:\n");
      for (j = 0; j < manif->apps.apps[i].icons.count; j++)
	printf ("        (%s) %s\n",
		nn (manif->apps.apps[i].icons.icons[j].section),
		nn (manif->apps.apps[i].icons.icons[j].name));
      printf ("      conditions:\n");
      for (j = 0; j < manif->apps.apps[i].launch_conditions.count; j++)
	printf ("        (%s) %s\n",
		nn (manif->apps.apps[i].launch_conditions.conditions[j].name),
		nn (manif->apps.apps[i].launch_conditions.
		    conditions[j].value));
      printf ("      notifications:\n");
      for (j = 0; j < manif->apps.apps[i].notifications.count; j++)
	printf ("        (%s) %s\n",
		nn (manif->apps.apps[i].notifications.notifications[j].name),
		nn (manif->apps.apps[i].notifications.
		    notifications[j].value));
      for (j = 0; j < manif->apps.apps[i].app_controls.count; j++)
	{
	  printf ("      app-controls %d: %s\n", j,
		  nn (manif->apps.apps[i].app_controls.app_controls[j].
		      provider_id));
	  for (k = 0;
	       k <
	       manif->apps.apps[i].app_controls.app_controls[j].capabilities.
	       count; k++)
	    {
	      printf ("        operation-id %s\n",
		      nn (manif->apps.apps[i].app_controls.app_controls[j].
			  capabilities.capabilities[k].operation_id));
	      for (l = 0;
		   l <
		   manif->apps.apps[i].app_controls.app_controls[j].
		   capabilities.capabilities[k].resolutions.count; l++)
		printf ("          (mime) %s (scheme) %s\n",
			nn (manif->apps.apps[i].app_controls.app_controls[j].
			    capabilities.capabilities[k].resolutions.
			    resolutions[l].mime_type),
			nn (manif->apps.apps[i].app_controls.app_controls[j].
			    capabilities.capabilities[k].resolutions.
			    resolutions[l].uri_scheme));

	    }
	}
      for (j = 0; j < manif->apps.apps[i].data_controls.count; j++)
	{
	  printf ("      data-controls %d: %s\n", j,
		  nn (manif->apps.apps[i].data_controls.data_controls[j].
		      provider_id));
	  for (k = 0;
	       k <
	       manif->apps.apps[i].data_controls.data_controls[j].types.count;
	       k++)
	    {
	      printf ("        (mime) %s (scheme) %s\n",
		      nn (manif->apps.apps[i].data_controls.data_controls[j].
			  types.types[k].access),
		      nn (manif->apps.apps[i].data_controls.data_controls[j].
			  types.types[k].value));

	    }
	}
    }
}

#undef nn
#endif

/*================== MAIN OF READING XML FILE =====================*/

static struct xml_read_elem *root[] = {
  &xre_Manifest
};

int
manifest_tpk_from_xml_file (struct manifest_tpk **manif, const char *path)
{
  struct manifest_tpk *tpk;

  /* allocate data */
  tpk = calloc (1, sizeof *tpk);
  if (tpk == NULL)
    return fail_out_of_memory ();

  /* parse */
  if (xml_read_file (path, root, 1, tpk))
    {
      free (tpk);
      return -1;
    }

#if !defined(NDEBUG)
  manifest_tpk_dump (tpk);
#endif
  *manif = tpk;
  return 0;
}

/*=========== UTILITY FUNCTIONS ===============*/

END (simple_pop)
{
  return xml_read_accept_pop (reader);
}

/* set a string data, emit an error if that data was already set */
static int
set_simple_string_once (struct xml_reader *reader, const char *ch, int len,
			struct manifest_tpk *manif, char **data,
			const char *name)
{
  if (*data)
    {
      /* TODO? log? warn? */
      errno = EEXIST;
      return -1;
    }
  *data = strndup (ch, len);
  if (!*data)
    return fail_out_of_memory ();
  return 0;
}

/* set an optional attribute data */
static int
set_optional_attribute (struct xml_reader *reader, const char *elem,
			const char **attrs, struct manifest_tpk *manif,
			char **data, const char *name)
{
  int status;

  assert (!*data);

  status = xml_read_attribute_copy (attrs, name, data);
  if (status < 0)
    return -1;
  assert ((status == 0) == (*data == NULL));
  return 0;
}

/* set a mandatory attribute data */
static int
set_mandatory_attribute (struct xml_reader *reader, const char *elem,
			 const char **attrs, struct manifest_tpk *manif,
			 char **data, const char *name)
{
  int status;

  assert (!*data);

  status = xml_read_attribute_copy (attrs, name, data);
  if (status < 0)
    return -1;
  if (status)
    return 0;
  /* TODO? log? warn? */
  errno = ENOENT;
  return -1;
}

static struct app *
new_app (struct manifest_tpk *manif, enum app_type type)
{
  int count;
  struct app *apps;

  count = manif->apps.count;
  apps = manif->apps.apps;

  apps = realloc (apps, (1 + count) * sizeof *apps);
  if (apps == NULL)
    {
      fail_out_of_memory ();
      return NULL;
    }

  memset (apps + count, 0, sizeof *apps);
  manif->apps.count = count + 1;
  manif->apps.apps = apps;

  apps[count].type = type;
  return apps + count;
}

/*================ Handling manifest ROOT element =====================*/

static struct xml_read_elem *children_of_manifest[] = {
  &xre_Id,
  &xre_Version,
  &xre_InstallationLocation,
  &xre_Type,
  &xre_Author,
  &xre_Descriptions,
  &xre_Url,
  &xre_Requirements,
  &xre_Apps
};

BEG (Manifest)
{
  int status;

  status =
    set_optional_attribute (reader, name, attrs, manif, &manif->locale,
			    "Locale");
  if (status)
    return status;
  return xml_read_accept_push (reader, children_of_manifest,
			       sizeof children_of_manifest /
			       sizeof *children_of_manifest);
}

END (Manifest)
{
  int status;

  status = xml_read_accept_pop (reader);
  if (status)
    return status;
  return xml_read_accept_drop (reader, &xre_Manifest);
}

CHA (Id)
{
  return set_simple_string_once (reader, ch, len, manif, &manif->id, "Id");
}

CHA (Version)
{
  return set_simple_string_once (reader, ch, len, manif, &manif->version,
				 "Version");
}

CHA (InstallationLocation)
{
  return set_simple_string_once (reader, ch, len, manif,
				 &manif->installation_location,
				 "InstallationLocation");
}

CHA (Type)
{
  return set_simple_string_once (reader, ch, len, manif, &manif->type,
				 "Type");
}

CHA (Url)
{
  return set_simple_string_once (reader, ch, len, manif, &manif->url, "Url");
}

CHA (Author)
{
  return set_simple_string_once (reader, ch, len, manif, &manif->author,
				 "Author");
}

/*================ Handling description list =====================*/

static struct xml_read_elem *children_of_descriptions[] = {
  &xre_Description
};

BEG (Descriptions)
{
  return xml_read_accept_push (reader, children_of_descriptions,
			       sizeof children_of_descriptions /
			       sizeof *children_of_descriptions);
}

BEG (Description)
{
  int count;
  struct locstr *descs;

  count = manif->descriptions.count;
  descs = manif->descriptions.descriptions;
  descs = realloc (descs, (1 + count) * sizeof *descs);
  if (descs == NULL)
    return fail_out_of_memory ();
  descs[count].locale = NULL;
  descs[count].value = NULL;
  manif->descriptions.descriptions = descs;
  manif->descriptions.count = 1 + count;
  return set_optional_attribute (reader, name, attrs, manif,
				 &descs[count].locale, "Locale");
}

CHA (Description)
{
  assert (manif->descriptions.count > 0);
  return set_simple_string_once (reader, ch, len, manif,
				 &manif->descriptions.descriptions[manif->
								   descriptions.
								   count -
								   1].value,
				 "Description");
}

/*================ Handling requirements list of feature =====================*/

static struct xml_read_elem *children_of_requirements[] = {
  &xre_Feature
};

BEG (Requirements)
{
  return xml_read_accept_push (reader, children_of_requirements,
			       sizeof children_of_requirements /
			       sizeof *children_of_requirements);
}

BEG (Feature)
{
  int count;
  struct named *feats;

  count = manif->features.count;
  feats = manif->features.features;
  feats = realloc (feats, (1 + count) * sizeof *feats);
  if (feats == NULL)
    return fail_out_of_memory ();
  feats[count].name = NULL;
  feats[count].value = NULL;
  manif->features.features = feats;
  manif->features.count = 1 + count;
  return set_optional_attribute (reader, name, attrs, manif,
				 &feats[count].name, "Name");
}

CHA (Feature)
{
  assert (manif->features.count > 0);

  return set_simple_string_once (reader, ch, len, manif,
				 &manif->features.features[manif->features.
							   count - 1].value,
				 "Feature");
}

/*================ Handling Apps =====================*/

static struct xml_read_elem *children_of_apps[] = {
  &xre_ApiVersion,
  &xre_Privileges,
  &xre_UiApp,
  &xre_ServiceApp
};

BEG (Apps)
{
  return xml_read_accept_push (reader, children_of_apps,
			       sizeof children_of_apps /
			       sizeof *children_of_apps);
}

CHA (ApiVersion)
{
  return set_simple_string_once (reader, ch, len, manif, &manif->api_version,
				 "ApiVersion");
}

/*================ Handling Privileges =====================*/

static struct xml_read_elem *children_of_privileges[] = {
  &xre_Privilege
};

BEG (Privileges)
{
  return xml_read_accept_push (reader, children_of_privileges,
			       sizeof children_of_privileges /
			       sizeof *children_of_privileges);
}

CHA (Privilege)
{
  int count;
  char **privileges;

  count = manif->privileges.count;
  privileges = manif->privileges.privileges;
  privileges = realloc (privileges, (1 + count) * sizeof *privileges);
  if (privileges == NULL)
    return fail_out_of_memory ();
  privileges[count] = NULL;
  manif->privileges.privileges = privileges;
  manif->privileges.count = 1 + count;
  return set_simple_string_once (reader, ch, len, manif,
				 &privileges[count], "Privilege");
}

/*================ Handling UiApp =====================*/

static struct xml_read_elem *children_of_ui_app[] = {
  &xre_DisplayNames,
  &xre_Icons,
  &xre_AppControls,
  &xre_LaunchConditions,
  &xre_Notifications,
  &xre_UiScalability,
  &xre_UiTheme,
  &xre_Ime,
  &xre_any_ok			/* Ime, Accounts */
};

BEG (UiApp)
{
  struct app *app;

  app = new_app (manif, ui_app);
  if (app == NULL)
    return -1;

  if (set_optional_attribute (reader, name, attrs, manif, &app->name, "Name")
      || set_optional_attribute (reader, name, attrs, manif, &app->main,
				 "Main")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->menu_icon_visible, "MenuIconVisible")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->launching_history_visible,
				 "LaunchingHistoryVisible")
      || set_optional_attribute (reader, name, attrs, manif, &app->category,
				 "Category")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->hw_acceleration, "HwAcceleration")
      || set_optional_attribute (reader, name, attrs, manif, &app->sub_mode,
				 "SubMode"))
    return -1;

  return xml_read_accept_push (reader, children_of_ui_app,
			       sizeof children_of_ui_app /
			       sizeof *children_of_ui_app);
}

/*================ Handling ServiceApp =====================*/

static struct xml_read_elem *children_of_service_app[] = {
  &xre_DisplayNames,
  &xre_Icons,
  &xre_AppControls,
  &xre_LaunchConditions,
  &xre_Notifications,
  &xre_DataControls,
  &xre_any_ok			/* AppWidgets */
};

BEG (ServiceApp)
{
  struct app *app;

  app = new_app (manif, service_app);
  if (app == NULL)
    return -1;

  if (set_optional_attribute (reader, name, attrs, manif, &app->name, "Name")
      || set_optional_attribute (reader, name, attrs, manif, &app->main,
				 "Main")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->launch_on_boot, "LaunchOnBoot")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->auto_restart, "AutoRestart")
      || set_optional_attribute (reader, name, attrs, manif, &app->use_ui,
				 "UseUi"))
    return -1;

  return xml_read_accept_push (reader, children_of_service_app,
			       sizeof children_of_service_app /
			       sizeof *children_of_service_app);
}

/*================ Handling DisplayNames =====================*/

static struct xml_read_elem *children_of_display_names[] = {
  &xre_DisplayName
};

BEG (DisplayNames)
{
  return xml_read_accept_push (reader, children_of_display_names,
			       sizeof children_of_display_names /
			       sizeof *children_of_display_names);
}

BEG (DisplayName)
{
  struct app *app;
  int count;
  struct locstr *dnames;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  count = app->display_names.count;
  dnames = app->display_names.display_names;
  dnames = realloc (dnames, (1 + count) * sizeof *dnames);
  if (dnames == NULL)
    return fail_out_of_memory ();
  dnames[count].locale = NULL;
  dnames[count].value = NULL;
  app->display_names.display_names = dnames;
  app->display_names.count = 1 + count;
  return set_optional_attribute (reader, name, attrs, manif,
				 &dnames[count].locale, "Locale");
}

CHA (DisplayName)
{
  struct app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->display_names.count > 0);
  return set_simple_string_once (reader, ch, len, manif,
				 &(app->display_names.
				   display_names[app->display_names.count -
						 1].value), "DisplayName");
}

/*================ Handling Icons =====================*/

static struct xml_read_elem *children_of_icons[] = {
  &xre_Icon
};

BEG (Icons)
{
  return xml_read_accept_push (reader, children_of_icons,
			       sizeof children_of_icons /
			       sizeof *children_of_icons);
}

BEG (Icon)
{
  struct app *app;
  int count;
  struct icon *icons;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  count = app->icons.count;
  icons = app->icons.icons;
  icons = realloc (icons, (1 + count) * sizeof *icons);
  if (icons == NULL)
    return fail_out_of_memory ();
  icons[count].section = NULL;
  icons[count].name = NULL;
  app->icons.icons = icons;
  app->icons.count = 1 + count;
  return set_optional_attribute (reader, name, attrs, manif,
				 &icons[count].section, "Section");
}

CHA (Icon)
{
  struct app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->icons.count > 0);
  return set_simple_string_once (reader, ch, len, manif,
				 &app->icons.icons[app->icons.count - 1].name,
				 "Icon");
}

/*================ Handling LaunchConditions =====================*/

static struct xml_read_elem *children_of_launch_conditions[] = {
  &xre_Condition
};

BEG (LaunchConditions)
{
  return xml_read_accept_push (reader, children_of_launch_conditions,
			       sizeof children_of_launch_conditions /
			       sizeof *children_of_launch_conditions);
}

BEG (Condition)
{
  struct app *app;
  int count;
  struct named *conditions;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  count = app->launch_conditions.count;
  conditions = app->launch_conditions.conditions;
  conditions = realloc (conditions, (1 + count) * sizeof *conditions);
  if (conditions == NULL)
    {
      return fail_out_of_memory ();
    }
  conditions[count].name = NULL;
  conditions[count].value = NULL;
  app->launch_conditions.conditions = conditions;
  app->launch_conditions.count = 1 + count;
  return set_optional_attribute (reader, name, attrs, manif,
				 &conditions[count].name, "Name");
}

CHA (Condition)
{
  struct app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->launch_conditions.count > 0);
  return set_simple_string_once (reader, ch, len, manif,
				 &app->launch_conditions.conditions[app->
								    launch_conditions.
								    count -
								    1].value,
				 "Condition");
}

/*================ Handling Notifications =====================*/

static struct xml_read_elem *children_of_notifications[] = {
  &xre_Notification
};

BEG (Notifications)
{
  return xml_read_accept_push (reader, children_of_notifications,
			       sizeof children_of_notifications /
			       sizeof *children_of_notifications);
}

BEG (Notification)
{
  struct app *app;
  int count;
  struct named *notifications;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  count = app->notifications.count;
  notifications = app->notifications.notifications;
  notifications =
    realloc (notifications, (1 + count) * sizeof *notifications);
  if (notifications == NULL)
    {
      return fail_out_of_memory ();
    }
  notifications[count].name = NULL;
  notifications[count].value = NULL;
  app->notifications.notifications = notifications;
  app->notifications.count = 1 + count;
  return set_optional_attribute (reader, name, attrs, manif,
				 &notifications[count].name, "Name");
}

CHA (Notification)
{
  struct app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->notifications.count > 0);
  return set_simple_string_once (reader, ch, len, manif,
				 &app->notifications.notifications[app->
								   notifications.
								   count -
								   1].value,
				 "notification");
}

/*================ Handling AppControls =====================*/

static struct xml_read_elem *children_of_app_controls[] = {
  &xre_AppControl
};

static struct xml_read_elem *children_of_app_control[] = {
  &xre_Capability
};

static struct xml_read_elem *children_of_capability[] = {
  &xre_Resolution
};

BEG (AppControls)
{
  return xml_read_accept_push (reader, children_of_app_controls,
			       sizeof children_of_app_controls /
			       sizeof *children_of_app_controls);
}

BEG (AppControl)
{
  struct app *app;
  int count;
  struct app_control *app_controls;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  count = app->app_controls.count;
  app_controls = app->app_controls.app_controls;
  app_controls = realloc (app_controls, (1 + count) * sizeof *app_controls);
  if (app_controls == NULL)
    return fail_out_of_memory ();

  memset (app_controls + count, 0, sizeof *app_controls);
  app->app_controls.app_controls = app_controls;
  app->app_controls.count = 1 + count;

  if (set_optional_attribute (reader, name, attrs, manif,
			      &app_controls[count].provider_id, "ProviderId"))
    return -1;

  return xml_read_accept_push (reader, children_of_app_control,
			       sizeof children_of_app_control /
			       sizeof *children_of_app_control);
}

BEG (Capability)
{
  struct app *app;
  struct app_control *app_control;
  struct capability *capabilities;
  int count;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->app_controls.count > 0);
  app_control = &app->app_controls.app_controls[app->app_controls.count - 1];

  count = app_control->capabilities.count;
  capabilities = app_control->capabilities.capabilities;
  capabilities = realloc (capabilities, (1 + count) * sizeof *capabilities);
  if (capabilities == NULL)
    return fail_out_of_memory ();

  memset (capabilities + count, 0, sizeof *capabilities);
  app_control->capabilities.capabilities = capabilities;
  app_control->capabilities.count = 1 + count;

  if (set_optional_attribute (reader, name, attrs, manif,
			      &capabilities[count].operation_id,
			      "OperationId"))
    return -1;

  return xml_read_accept_push (reader, children_of_capability,
			       sizeof children_of_capability /
			       sizeof *children_of_capability);
}

BEG (Resolution)
{
  struct app *app;
  struct app_control *app_control;
  struct capability *capability;
  struct resolution *resolutions;
  int count;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->app_controls.count > 0);
  app_control = &app->app_controls.app_controls[app->app_controls.count - 1];
  assert (app_control->capabilities.count > 0);
  capability =
    &app_control->capabilities.capabilities[app_control->capabilities.count -
					    1];

  count = capability->resolutions.count;
  resolutions = capability->resolutions.resolutions;
  resolutions = realloc (resolutions, (1 + count) * sizeof *resolutions);
  if (resolutions == NULL)
    return fail_out_of_memory ();

  memset (resolutions + count, 0, sizeof *resolutions);
  capability->resolutions.resolutions = resolutions;
  capability->resolutions.count = 1 + count;

  if (set_optional_attribute (reader, name, attrs, manif,
			      &resolutions[count].mime_type, "MimeType")
      || set_optional_attribute (reader, name, attrs, manif,
				 &resolutions[count].uri_scheme, "UriScheme"))
    return -1;

  return 0;
}

/*================ Handling DataControls =====================*/

static struct xml_read_elem *children_of_data_controls[] = {
  &xre_DataControl
};

static struct xml_read_elem *children_of_data_control[] = {
  &xre_DataControlType
};

BEG (DataControls)
{
  return xml_read_accept_push (reader, children_of_data_controls,
			       sizeof children_of_data_controls /
			       sizeof *children_of_data_controls);
}

BEG (DataControl)
{
  struct app *app;
  int count;
  struct data_control *data_controls;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  count = app->data_controls.count;
  data_controls = app->data_controls.data_controls;
  data_controls =
    realloc (data_controls, (1 + count) * sizeof *data_controls);
  if (data_controls == NULL)
    return fail_out_of_memory ();

  memset (data_controls + count, 0, sizeof *data_controls);
  app->data_controls.data_controls = data_controls;
  app->data_controls.count = 1 + count;

  if (set_optional_attribute (reader, name, attrs, manif,
			      &data_controls[count].provider_id,
			      "ProviderId"))
    return -1;

  return xml_read_accept_push (reader, children_of_data_control,
			       sizeof children_of_data_control /
			       sizeof *children_of_data_control);
}

BEG (DataControlType)
{
  struct app *app;
  struct data_control *data_control;
  struct data_control_type *types;
  int count;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->data_controls.count > 0);
  data_control =
    &app->data_controls.data_controls[app->data_controls.count - 1];

  count = data_control->types.count;
  types = data_control->types.types;
  types = realloc (types, (1 + count) * sizeof *types);
  if (types == NULL)
    return fail_out_of_memory ();

  memset (types + count, 0, sizeof *types);
  data_control->types.types = types;
  data_control->types.count = 1 + count;

  return set_optional_attribute (reader, name, attrs, manif,
				 &types[count].access, "Access");
}

CHA (DataControlType)
{
  struct app *app;
  struct data_control *data_control;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->data_controls.count > 0);
  data_control =
    &app->data_controls.data_controls[app->data_controls.count - 1];
  assert (data_control->types.count > 0);

  return set_simple_string_once (reader, ch, len, manif,
				 &data_control->types.
				 types[data_control->types.count - 1].value,
				 "DataControlType");
}

/*================ Handling UiScalability and UiTheme =====================*/

BEG (UiScalability)
{
  struct app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];

  if (set_optional_attribute
      (reader, name, attrs, manif, &app->ui_scalability.coordinate_system,
       "CoordinateSystem")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->ui_scalability.base_screen_size,
				 "BaseScreenSize")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->ui_scalability.logical_coordinate,
				 "LogicalCoordinate"))
    return -1;

  return 0;
}

BEG (UiTheme)
{
  struct app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];

  if (set_optional_attribute
      (reader, name, attrs, manif, &app->ui_theme.system_theme, "SystemTheme")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->ui_theme.user_defined_theme,
				 "UserDefinedTheme"))
    return -1;

  return 0;
}

/*================ Handling Ime =====================*/

static struct xml_read_elem *children_of_ime[] = {
  &xre_Uuid,
  &xre_Languages
};

static struct xml_read_elem *children_of_languages[] = {
  &xre_Language
};

BEG (Ime)
{
  return xml_read_accept_push (reader, children_of_ime,
			       sizeof children_of_ime /
			       sizeof *children_of_ime);
}

BEG (Languages)
{
  return xml_read_accept_push (reader, children_of_languages,
			       sizeof children_of_languages /
			       sizeof *children_of_languages);
}

CHA (Uuid)
{
  struct app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  return set_simple_string_once (reader, ch, len, manif,
				 &app->ime.uuid, "Uuid");
}

CHA (Language)
{
  struct app *app;
  int count;
  char **languages;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  count = app->ime.languages.count;
  languages = app->ime.languages.languages;
  languages = realloc (languages, (1 + count) * sizeof *languages);
  if (languages == NULL)
    return fail_out_of_memory ();
  app->ime.languages.count = count + 1;
  app->ime.languages.languages = languages;
  languages[count] = NULL;
  return set_simple_string_once (reader, ch, len, manif,
				 &languages[count], "Language");
}

/*================ Handling Accounts =====================*/




/*================ Handling ? =====================*/
/*================ Handling ? =====================*/
/*================ Handling ? =====================*/
/*================ Handling ? =====================*/
/*================ Handling ? =====================*/
/*================ Handling ? =====================*/
/*================ Handling ? =====================*/
