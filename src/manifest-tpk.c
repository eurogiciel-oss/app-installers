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

END (simple_pop);

#define _XRE_B($id,$tag) BEG($id); _XRE($id,$tag,beg_##$id,0,0)
#define _XRE_E($id,$tag) END($id); _XRE($id,$tag,0,end_##$id,0)
#define _XRE_C($id,$tag) CHA($id); _XRE($id,$tag,0,0,cha_##$id)
#define _XRE_BE($id,$tag) BEG($id); END($id); _XRE($id,$tag,beg_##$id,end_##$id,0)
#define _XRE_BC($id,$tag) BEG($id); CHA($id); _XRE($id,$tag,beg_##$id,0,cha_##$id)
#define _XRE_EC($id,$tag) END($id); CHA($id); _XRE($id,$tag,0,end_##$id,cha_##$id)
#define _XRE_BEC($id,$tag) BEG($id); END($id); CHA($id); _XRE($id,$tag,beg_##$id,end_##$id,cha_##$id)
#define _XRE_B_($id,$tag) BEG($id); _XRE($id,$tag,beg_##$id,end_simple_pop,0)

#define XRE($id,$begin,$end,$characters) _XRE($id,$id,$begin,$end,$characters)

#define XRE_B($id) _XRE_B($id,$id)
#define XRE_E($id) _XRE_E($id,$id)
#define XRE_C($id) _XRE_C($id,$id)
#define XRE_BE($id) _XRE_BE($id,$id)
#define XRE_BC($id) _XRE_BC($id,$id)
#define XRE_EC($id) _XRE_EC($id,$id)
#define XRE_BEC($id) _XRE_BEC($id,$id)
#define XRE_B_($id) _XRE_B_($id,$id)

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

_XRE_B_ (app_DisplayNames, DisplayNames);
_XRE_BC (app_DisplayName, DisplayName);
_XRE_B_ (app_Icons, Icons);
_XRE_BC (app_Icon, Icon);
XRE_B_ (LaunchConditions);
XRE_BC (Condition);
XRE_B_ (Notifications);
XRE_BC (Notification);
XRE_B_ (AppControls);
XRE_B_ (AppControl);
XRE_B_ (Capability);
XRE_B (Resolution);

XRE_B_ (UiApp);

XRE_B (UiScalability);
XRE_B (UiTheme);

XRE_B_ (Ime);
XRE_C (Uuid);
XRE_B_ (Languages);
XRE_C (Language);

XRE_B_ (Accounts);
XRE_B_ (AccountProvider);
_XRE_B_ (account_Icons, Icons);
_XRE_BC (account_Icon, Icon);
_XRE_B_ (account_DisplayNames, DisplayNames);
_XRE_BC (account_DisplayName, DisplayName);
XRE_B_ (Capabilities);
_XRE_C (account_Capability, Capability);

XRE_B_ (ServiceApp);

XRE_B_ (DataControls);
XRE_B_ (DataControl);
XRE_BC (DataControlType);

XRE_B_ (AppWidgets);
XRE_B_ (AppWidget);
_XRE_B_ (widget_DisplayNames, DisplayNames);
_XRE_BC (widget_DisplayName, DisplayName);
XRE_B_ (Sizes);
XRE_BC (Size);
XRE_C (ConfigurationAppControlAppId);

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
    printf ("    %s: %s\n", nn (manif->features.features[i].name), nn (manif->features.features[i].value));
  printf ("  APPS:\n");
  printf ("    api_version............... %s\n", nn (manif->api_version));
  printf ("    privileges:\n");
  for (j = 0; j < manif->privileges.count; j++)
    printf ("      ... %s\n", nn (manif->privileges.privileges[j]));
  for (i = 0; i < manif->apps.count; i++)
    {
      struct tpk_app *app = &manif->apps.apps[i];

      /* common of applications */
      printf ("    APP %d:\n", i);
      printf ("      name...................... %s\n", nn (app->name));
      printf ("      type...................... %d\n", app->type);
      printf ("      main...................... %s\n", nn (app->main));
      printf ("      name...................... %s\n", nn (app->name));
      printf ("      name...................... %s\n", nn (app->name));

      printf ("      display names:\n");
      for (j = 0; j < app->display_names.count; j++)
	printf ("        (%s) %s\n",
		nn (app->display_names.display_names[j].locale), nn (app->display_names.display_names[j].value));
      printf ("      icons:\n");
      for (j = 0; j < app->icons.count; j++)
	printf ("        (%s) %s\n", nn (app->icons.icons[j].section), nn (app->icons.icons[j].name));
      printf ("      conditions:\n");
      for (j = 0; j < app->launch_conditions.count; j++)
	printf ("        (%s) %s\n",
		nn (app->launch_conditions.conditions[j].name), nn (app->launch_conditions.conditions[j].value));
      printf ("      notifications:\n");
      for (j = 0; j < app->notifications.count; j++)
	printf ("        (%s) %s\n",
		nn (app->notifications.notifications[j].name), nn (app->notifications.notifications[j].value));
      for (j = 0; j < app->app_controls.count; j++)
	{
	  printf ("      app-controls %d: %s\n", j, nn (app->app_controls.app_controls[j].provider_id));
	  for (k = 0; k < app->app_controls.app_controls[j].capabilities.count; k++)
	    {
	      printf ("        operation-id %s\n",
		      nn (app->app_controls.app_controls[j].capabilities.capabilities[k].operation_id));
	      for (l = 0; l < app->app_controls.app_controls[j].capabilities.capabilities[k].resolutions.count; l++)
		printf ("          (mime) %s (scheme) %s\n",
			nn (app->app_controls.app_controls[j].capabilities.capabilities[k].resolutions.
			    resolutions[l].mime_type),
			nn (app->app_controls.app_controls[j].capabilities.capabilities[k].resolutions.
			    resolutions[l].uri_scheme));

	    }
	}

      /* specific of UI applications */
      printf ("      menu_icon_visible......... %s\n", nn (app->ui_app.menu_icon_visible));
      printf ("      launching_history_visible. %s\n", nn (app->ui_app.launching_history_visible));
      printf ("      category.................. %s\n", nn (app->ui_app.category));
      printf ("      hw_acceleration........... %s\n", nn (app->ui_app.hw_acceleration));
      printf ("      sub_mode.................. %s\n", nn (app->ui_app.sub_mode));
      printf ("      coordinate system......... %s\n", nn (app->ui_app.ui_scalability.coordinate_system));
      printf ("      base screen size.......... %s\n", nn (app->ui_app.ui_scalability.base_screen_size));
      printf ("      logical coordinate........ %s\n", nn (app->ui_app.ui_scalability.logical_coordinate));

      printf ("      system theme.............. %s\n", nn (app->ui_app.ui_theme.system_theme));
      printf ("      user defined theme........ %s\n", nn (app->ui_app.ui_theme.user_defined_theme));

      printf ("      ime uuid.................. %s\n", nn (app->ui_app.ime.uuid));
      printf ("      ui_app.ime languages.............");
      for (j = 0; j < app->ui_app.ime.languages.count; j++)
	printf (" %s", nn (app->ui_app.ime.languages.languages[j]));
      printf ("\n");

      for (j = 0; j < app->ui_app.accounts.count; j++)
	{
	  struct tpk_account_provider *account = &app->ui_app.accounts.accounts[j];

	  printf ("      account %d\n", j);
	  printf ("        multiple_account_support... %s\n", nn (account->multiple_account_support));
	  printf ("        icons:\n");
	  for (k = 0; k < account->icons.count; k++)
	    printf ("          (%s) %s\n", nn (account->icons.icons[k].section), nn (account->icons.icons[k].name));
	  printf ("        display names:\n");
	  for (k = 0; k < account->display_names.count; k++)
	    printf ("          (%s) %s\n",
		    nn (account->display_names.display_names[k].locale),
		    nn (account->display_names.display_names[k].value));
	  printf ("        capabilities:\n");
	  for (k = 0; k < account->capabilities.count; k++)
	    printf ("          %s\n", nn (account->capabilities.capabilities[k]));
	}

      /* specific of Service applications */
      printf ("      launch_on_boot............ %s\n", nn (app->service_app.launch_on_boot));
      printf ("      auto_restart.............. %s\n", nn (app->service_app.auto_restart));
      printf ("      use_ui.................... %s\n", nn (app->service_app.use_ui));
      for (j = 0; j < app->service_app.data_controls.count; j++)
	{
	  printf ("      data-controls %d: %s\n", j, nn (app->service_app.data_controls.data_controls[j].provider_id));
	  for (k = 0; k < app->service_app.data_controls.data_controls[j].types.count; k++)
	    {
	      printf ("        (mime) %s (scheme) %s\n",
		      nn (app->service_app.data_controls.data_controls[j].types.types[k].access),
		      nn (app->service_app.data_controls.data_controls[j].types.types[k].value));

	    }
	}
      for (j = 0; j < app->service_app.app_widgets.count; j++)
	{
	  struct tpk_app_widget *widget = &app->service_app.app_widgets.app_widgets[j];
	  printf ("      app widget %d:\n", j);
	  printf ("        app widget popup enable........... %s\n", nn (widget->app_widget_popup_enable));
	  printf ("        provider name..................... %s\n", nn (widget->provider_name));
	  printf ("        update period..................... %s\n", nn (widget->update_period));
	  printf ("        default........................... %s\n", nn (widget->default_));
	  printf ("        configuration app control app id.. %s\n", nn (widget->configuration_app_control_app_id));
	  printf ("        display names:\n");
	  for (k = 0; k < widget->display_names.count; k++)
	    printf ("          (%s) %s\n",
		    nn (widget->display_names.display_names[k].locale),
		    nn (widget->display_names.display_names[k].value));
	  printf ("        sizes:\n");
	  for (k = 0; k < widget->sizes.count; k++)
	    {
	      printf ("          size %d\n", k);
	      printf ("            preview image......... %s\n", nn (widget->sizes.sizes[k].preview_image));
	      printf ("            use decoration frame.. %s\n", nn (widget->sizes.sizes[k].use_decoration_frame));
	      printf ("            value................. %s\n", nn (widget->sizes.sizes[k].value));
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
			struct manifest_tpk *manif, char **data, const char *name)
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
			const char **attrs, struct manifest_tpk *manif, char **data, const char *name)
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
			 const char **attrs, struct manifest_tpk *manif, char **data, const char *name)
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

static struct tpk_app *
new_app (struct manifest_tpk *manif, enum app_type type)
{
  int count;
  struct tpk_app *apps;

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

  status = set_optional_attribute (reader, name, attrs, manif, &manif->locale, "Locale");
  if (status)
    return status;
  return xml_read_accept_push (reader, children_of_manifest,
			       sizeof children_of_manifest / sizeof *children_of_manifest);
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
  return set_simple_string_once (reader, ch, len, manif, &manif->version, "Version");
}

CHA (InstallationLocation)
{
  return set_simple_string_once (reader, ch, len, manif, &manif->installation_location, "InstallationLocation");
}

CHA (Type)
{
  return set_simple_string_once (reader, ch, len, manif, &manif->type, "Type");
}

CHA (Url)
{
  return set_simple_string_once (reader, ch, len, manif, &manif->url, "Url");
}

CHA (Author)
{
  return set_simple_string_once (reader, ch, len, manif, &manif->author, "Author");
}

/*================ Handling description list =====================*/

static struct xml_read_elem *children_of_descriptions[] = {
  &xre_Description
};

BEG (Descriptions)
{
  return xml_read_accept_push (reader, children_of_descriptions,
			       sizeof children_of_descriptions / sizeof *children_of_descriptions);
}

BEG (Description)
{
  int count;
  struct tpk_locstr *descs;

  count = manif->descriptions.count;
  descs = manif->descriptions.descriptions;
  descs = realloc (descs, (1 + count) * sizeof *descs);
  if (descs == NULL)
    return fail_out_of_memory ();
  descs[count].locale = NULL;
  descs[count].value = NULL;
  manif->descriptions.descriptions = descs;
  manif->descriptions.count = 1 + count;
  return set_optional_attribute (reader, name, attrs, manif, &descs[count].locale, "Locale");
}

CHA (Description)
{
  assert (manif->descriptions.count > 0);
  return set_simple_string_once (reader, ch, len, manif,
				 &manif->descriptions.descriptions[manif->descriptions.count - 1].value, "Description");
}

/*================ Handling requirements list of feature =====================*/

static struct xml_read_elem *children_of_requirements[] = {
  &xre_Feature
};

BEG (Requirements)
{
  return xml_read_accept_push (reader, children_of_requirements,
			       sizeof children_of_requirements / sizeof *children_of_requirements);
}

BEG (Feature)
{
  int count;
  struct tpk_named *feats;

  count = manif->features.count;
  feats = manif->features.features;
  feats = realloc (feats, (1 + count) * sizeof *feats);
  if (feats == NULL)
    return fail_out_of_memory ();
  feats[count].name = NULL;
  feats[count].value = NULL;
  manif->features.features = feats;
  manif->features.count = 1 + count;
  return set_optional_attribute (reader, name, attrs, manif, &feats[count].name, "Name");
}

CHA (Feature)
{
  assert (manif->features.count > 0);

  return set_simple_string_once (reader, ch, len, manif,
				 &manif->features.features[manif->features.count - 1].value, "Feature");
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
  return xml_read_accept_push (reader, children_of_apps, sizeof children_of_apps / sizeof *children_of_apps);
}

CHA (ApiVersion)
{
  return set_simple_string_once (reader, ch, len, manif, &manif->api_version, "ApiVersion");
}

/*================ Handling Privileges =====================*/

static struct xml_read_elem *children_of_privileges[] = {
  &xre_Privilege
};

BEG (Privileges)
{
  return xml_read_accept_push (reader, children_of_privileges,
			       sizeof children_of_privileges / sizeof *children_of_privileges);
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
  return set_simple_string_once (reader, ch, len, manif, &privileges[count], "Privilege");
}

/*================ Handling UiApp =====================*/

static struct xml_read_elem *children_of_ui_app[] = {
  &xre_app_DisplayNames,
  &xre_app_Icons,
  &xre_AppControls,
  &xre_LaunchConditions,
  &xre_Notifications,
  &xre_UiScalability,
  &xre_UiTheme,
  &xre_Ime,
  &xre_Accounts,
  &xre_any_ok			/* Accounts */
};

BEG (UiApp)
{
  struct tpk_app *app;

  app = new_app (manif, ui_app);
  if (app == NULL)
    return -1;

  if (set_optional_attribute (reader, name, attrs, manif, &app->name, "Name")
      || set_optional_attribute (reader, name, attrs, manif, &app->main,
				 "Main")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->ui_app.menu_icon_visible,
				 "MenuIconVisible")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->ui_app.launching_history_visible,
				 "LaunchingHistoryVisible")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->ui_app.category, "Category")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->ui_app.hw_acceleration,
				 "HwAcceleration")
      || set_optional_attribute (reader, name, attrs, manif, &app->ui_app.sub_mode, "SubMode"))
    return -1;

  return xml_read_accept_push (reader, children_of_ui_app, sizeof children_of_ui_app / sizeof *children_of_ui_app);
}

/*================ Handling ServiceApp =====================*/

static struct xml_read_elem *children_of_service_app[] = {
  &xre_app_DisplayNames,
  &xre_app_Icons,
  &xre_AppControls,
  &xre_LaunchConditions,
  &xre_Notifications,
  &xre_DataControls,
  &xre_AppWidgets,
  &xre_any_ok			/* AppWidgets */
};

BEG (ServiceApp)
{
  struct tpk_app *app;

  app = new_app (manif, service_app);
  if (app == NULL)
    return -1;

  if (set_optional_attribute (reader, name, attrs, manif, &app->name, "Name")
      || set_optional_attribute (reader, name, attrs, manif, &app->main,
				 "Main")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->service_app.launch_on_boot,
				 "LaunchOnBoot")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->service_app.auto_restart,
				 "AutoRestart")
      || set_optional_attribute (reader, name, attrs, manif, &app->service_app.use_ui, "UseUi"))
    return -1;

  return xml_read_accept_push (reader, children_of_service_app,
			       sizeof children_of_service_app / sizeof *children_of_service_app);
}

/*================ Handling DisplayNames =====================*/

static int
begin_display_name (struct xml_reader *reader, const char *name,
		    const char **attrs, struct manifest_tpk *manif, struct tpk_display_names *display_names)
{
  int count;
  struct tpk_locstr *list;

  count = display_names->count;
  list = display_names->display_names;
  list = realloc (list, (1 + count) * sizeof *list);
  if (list == NULL)
    return fail_out_of_memory ();
  list[count].locale = NULL;
  list[count].value = NULL;
  display_names->display_names = list;
  display_names->count = 1 + count;
  return set_optional_attribute (reader, name, attrs, manif, &list[count].locale, "Locale");
}

static int
character_display_name (struct xml_reader *reader, const char *ch, int len,
			struct manifest_tpk *manif, struct tpk_display_names *display_names)
{
  assert (display_names->count > 0);
  return set_simple_string_once (reader, ch, len, manif,
				 &display_names->display_names[display_names->count - 1].value, "DisplayName");
}

static struct xml_read_elem *children_of_app_display_names[] = {
  &xre_app_DisplayName
};

BEG (app_DisplayNames)
{
  return xml_read_accept_push (reader, children_of_app_display_names,
			       sizeof children_of_app_display_names / sizeof *children_of_app_display_names);
}

BEG (app_DisplayName)
{
  struct tpk_app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  return begin_display_name (reader, name, attrs, manif, &app->display_names);
}

CHA (app_DisplayName)
{
  struct tpk_app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  return character_display_name (reader, ch, len, manif, &app->display_names);
}

static struct xml_read_elem *children_of_account_display_names[] = {
  &xre_account_DisplayName
};

BEG (account_DisplayNames)
{
  return xml_read_accept_push (reader, children_of_account_display_names,
			       sizeof children_of_account_display_names / sizeof *children_of_account_display_names);
}

BEG (account_DisplayName)
{
  struct tpk_app *app;
  struct tpk_account_provider *account;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->ui_app.accounts.count > 0);
  account = &app->ui_app.accounts.accounts[app->ui_app.accounts.count - 1];
  return begin_display_name (reader, name, attrs, manif, &account->display_names);
}

CHA (account_DisplayName)
{
  struct tpk_app *app;
  struct tpk_account_provider *account;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->ui_app.accounts.count > 0);
  account = &app->ui_app.accounts.accounts[app->ui_app.accounts.count - 1];
  return character_display_name (reader, ch, len, manif, &account->display_names);
}

static struct xml_read_elem *children_of_widget_display_names[] = {
  &xre_widget_DisplayName
};

BEG (widget_DisplayNames)
{
  return xml_read_accept_push (reader, children_of_widget_display_names,
			       sizeof children_of_widget_display_names / sizeof *children_of_widget_display_names);
}

BEG (widget_DisplayName)
{
  struct tpk_app *app;
  struct tpk_app_widget *app_widget;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->service_app.app_widgets.count > 0);
  app_widget = &app->service_app.app_widgets.app_widgets[app->service_app.app_widgets.count - 1];

  return begin_display_name (reader, name, attrs, manif, &app_widget->display_names);
}

CHA (widget_DisplayName)
{
  struct tpk_app *app;
  struct tpk_app_widget *app_widget;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->service_app.app_widgets.count > 0);
  app_widget = &app->service_app.app_widgets.app_widgets[app->service_app.app_widgets.count - 1];

  return character_display_name (reader, ch, len, manif, &app_widget->display_names);
}

/*================ Handling Icons =====================*/

static int
begin_icon (struct xml_reader *reader, const char *name, const char **attrs,
	    struct manifest_tpk *manif, struct tpk_icons *icons)
{
  int count;
  struct tpk_icon *list;

  count = icons->count;
  list = icons->icons;
  list = realloc (list, (1 + count) * sizeof *list);
  if (list == NULL)
    return fail_out_of_memory ();
  list[count].section = NULL;
  list[count].name = NULL;
  icons->icons = list;
  icons->count = 1 + count;
  return set_optional_attribute (reader, name, attrs, manif, &list[count].section, "Section");
}

static int
character_icon (struct xml_reader *reader, const char *ch, int len, struct manifest_tpk *manif, struct tpk_icons *icons)
{
  assert (icons->count > 0);
  return set_simple_string_once (reader, ch, len, manif, &icons->icons[icons->count - 1].name, "Icon");
}

static struct xml_read_elem *children_of_app_icons[] = {
  &xre_app_Icon
};

BEG (app_Icons)
{
  return xml_read_accept_push (reader, children_of_app_icons,
			       sizeof children_of_app_icons / sizeof *children_of_app_icons);
}

BEG (app_Icon)
{
  struct tpk_app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  return begin_icon (reader, name, attrs, manif, &app->icons);
}

CHA (app_Icon)
{
  struct tpk_app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  return character_icon (reader, ch, len, manif, &app->icons);
}

static struct xml_read_elem *children_of_account_icons[] = {
  &xre_account_Icon
};

BEG (account_Icons)
{
  return xml_read_accept_push (reader, children_of_account_icons,
			       sizeof children_of_account_icons / sizeof *children_of_account_icons);
}

BEG (account_Icon)
{
  struct tpk_app *app;
  struct tpk_account_provider *account;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->ui_app.accounts.count > 0);
  account = &app->ui_app.accounts.accounts[app->ui_app.accounts.count - 1];
  return begin_icon (reader, name, attrs, manif, &account->icons);
}

CHA (account_Icon)
{
  struct tpk_app *app;
  struct tpk_account_provider *account;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->ui_app.accounts.count > 0);
  account = &app->ui_app.accounts.accounts[app->ui_app.accounts.count - 1];
  return character_icon (reader, ch, len, manif, &account->icons);
}

/*================ Handling LaunchConditions =====================*/

static struct xml_read_elem *children_of_launch_conditions[] = {
  &xre_Condition
};

BEG (LaunchConditions)
{
  return xml_read_accept_push (reader, children_of_launch_conditions,
			       sizeof children_of_launch_conditions / sizeof *children_of_launch_conditions);
}

BEG (Condition)
{
  struct tpk_app *app;
  int count;
  struct tpk_named *conditions;

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
  return set_optional_attribute (reader, name, attrs, manif, &conditions[count].name, "Name");
}

CHA (Condition)
{
  struct tpk_app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->launch_conditions.count > 0);
  return set_simple_string_once (reader, ch, len, manif,
				 &app->launch_conditions.conditions[app->launch_conditions.count -
								    1].value, "Condition");
}

/*================ Handling Notifications =====================*/

static struct xml_read_elem *children_of_notifications[] = {
  &xre_Notification
};

BEG (Notifications)
{
  return xml_read_accept_push (reader, children_of_notifications,
			       sizeof children_of_notifications / sizeof *children_of_notifications);
}

BEG (Notification)
{
  struct tpk_app *app;
  int count;
  struct tpk_named *notifications;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  count = app->notifications.count;
  notifications = app->notifications.notifications;
  notifications = realloc (notifications, (1 + count) * sizeof *notifications);
  if (notifications == NULL)
    {
      return fail_out_of_memory ();
    }
  notifications[count].name = NULL;
  notifications[count].value = NULL;
  app->notifications.notifications = notifications;
  app->notifications.count = 1 + count;
  return set_optional_attribute (reader, name, attrs, manif, &notifications[count].name, "Name");
}

CHA (Notification)
{
  struct tpk_app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->notifications.count > 0);
  return set_simple_string_once (reader, ch, len, manif,
				 &app->notifications.notifications[app->notifications.count - 1].value, "notification");
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
			       sizeof children_of_app_controls / sizeof *children_of_app_controls);
}

BEG (AppControl)
{
  struct tpk_app *app;
  int count;
  struct tpk_app_control *app_controls;

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

  if (set_optional_attribute (reader, name, attrs, manif, &app_controls[count].provider_id, "ProviderId"))
    return -1;

  return xml_read_accept_push (reader, children_of_app_control,
			       sizeof children_of_app_control / sizeof *children_of_app_control);
}

BEG (Capability)
{
  struct tpk_app *app;
  struct tpk_app_control *app_control;
  struct tpk_app_capability *capabilities;
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

  if (set_optional_attribute (reader, name, attrs, manif, &capabilities[count].operation_id, "OperationId"))
    return -1;

  return xml_read_accept_push (reader, children_of_capability,
			       sizeof children_of_capability / sizeof *children_of_capability);
}

BEG (Resolution)
{
  struct tpk_app *app;
  struct tpk_app_control *app_control;
  struct tpk_app_capability *capability;
  struct tpk_resolution *resolutions;
  int count;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->app_controls.count > 0);
  app_control = &app->app_controls.app_controls[app->app_controls.count - 1];
  assert (app_control->capabilities.count > 0);
  capability = &app_control->capabilities.capabilities[app_control->capabilities.count - 1];

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
      || set_optional_attribute (reader, name, attrs, manif, &resolutions[count].uri_scheme, "UriScheme"))
    return -1;

  return 0;
}

/*================ Handling UiScalability and UiTheme =====================*/

BEG (UiScalability)
{
  struct tpk_app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];

  if (set_optional_attribute
      (reader, name, attrs, manif,
       &app->ui_app.ui_scalability.coordinate_system, "CoordinateSystem")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->ui_app.ui_scalability.base_screen_size,
				 "BaseScreenSize")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->ui_app.ui_scalability.logical_coordinate, "LogicalCoordinate"))
    return -1;

  return 0;
}

BEG (UiTheme)
{
  struct tpk_app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];

  if (set_optional_attribute
      (reader, name, attrs, manif, &app->ui_app.ui_theme.system_theme,
       "SystemTheme")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app->ui_app.ui_theme.user_defined_theme, "UserDefinedTheme"))
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
  return xml_read_accept_push (reader, children_of_ime, sizeof children_of_ime / sizeof *children_of_ime);
}

BEG (Languages)
{
  return xml_read_accept_push (reader, children_of_languages,
			       sizeof children_of_languages / sizeof *children_of_languages);
}

CHA (Uuid)
{
  struct tpk_app *app;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  return set_simple_string_once (reader, ch, len, manif, &app->ui_app.ime.uuid, "Uuid");
}

CHA (Language)
{
  struct tpk_app *app;
  int count;
  char **languages;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  count = app->ui_app.ime.languages.count;
  languages = app->ui_app.ime.languages.languages;
  languages = realloc (languages, (1 + count) * sizeof *languages);
  if (languages == NULL)
    return fail_out_of_memory ();
  app->ui_app.ime.languages.count = count + 1;
  app->ui_app.ime.languages.languages = languages;
  languages[count] = NULL;
  return set_simple_string_once (reader, ch, len, manif, &languages[count], "Language");
}

/*================ Handling Accounts =====================*/

static struct xml_read_elem *children_of_accounts[] = {
  &xre_AccountProvider
};

static struct xml_read_elem *children_of_account_provider[] = {
  &xre_account_Icons,
  &xre_account_DisplayNames,
  &xre_Capabilities
};

static struct xml_read_elem *children_of_capabilities[] = {
  &xre_account_Capability
};


BEG (Accounts)
{
  return xml_read_accept_push (reader, children_of_accounts,
			       sizeof children_of_accounts / sizeof *children_of_accounts);
}

BEG (AccountProvider)
{
  struct tpk_app *app;
  int count;
  struct tpk_account_provider *accounts;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  count = app->ui_app.accounts.count;
  accounts = app->ui_app.accounts.accounts;
  accounts = realloc (accounts, (1 + count) * sizeof *accounts);
  if (accounts == NULL)
    return fail_out_of_memory ();

  memset (accounts + count, 0, sizeof *accounts);
  app->ui_app.accounts.accounts = accounts;
  app->ui_app.accounts.count = 1 + count;

  if (set_optional_attribute (reader, name, attrs, manif,
			      &accounts[count].multiple_account_support, "MultipleAccountSupport"))
    return -1;
  return xml_read_accept_push (reader, children_of_account_provider,
			       sizeof children_of_account_provider / sizeof *children_of_account_provider);
}

BEG (Capabilities)
{
  return xml_read_accept_push (reader, children_of_capabilities,
			       sizeof children_of_capabilities / sizeof *children_of_capabilities);
}

CHA (account_Capability)
{
  struct tpk_app *app;
  struct tpk_account_provider *account;
  char **capabilities;
  int count;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->ui_app.accounts.count > 0);
  account = &app->ui_app.accounts.accounts[app->ui_app.accounts.count - 1];
  count = account->capabilities.count;
  capabilities = account->capabilities.capabilities;

  capabilities = realloc (capabilities, (1 + count) * sizeof *capabilities);
  if (capabilities == NULL)
    return fail_out_of_memory ();
  account->capabilities.capabilities = capabilities;
  account->capabilities.count = 1 + count;
  capabilities[count] = NULL;
  return set_simple_string_once (reader, ch, len, manif, &capabilities[count], "Capability");
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
			       sizeof children_of_data_controls / sizeof *children_of_data_controls);
}

BEG (DataControl)
{
  struct tpk_app *app;
  int count;
  struct tpk_data_control *data_controls;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  count = app->service_app.data_controls.count;
  data_controls = app->service_app.data_controls.data_controls;
  data_controls = realloc (data_controls, (1 + count) * sizeof *data_controls);
  if (data_controls == NULL)
    return fail_out_of_memory ();

  memset (data_controls + count, 0, sizeof *data_controls);
  app->service_app.data_controls.data_controls = data_controls;
  app->service_app.data_controls.count = 1 + count;

  if (set_optional_attribute (reader, name, attrs, manif, &data_controls[count].provider_id, "ProviderId"))
    return -1;

  return xml_read_accept_push (reader, children_of_data_control,
			       sizeof children_of_data_control / sizeof *children_of_data_control);
}

BEG (DataControlType)
{
  struct tpk_app *app;
  struct tpk_data_control *data_control;
  struct tpk_data_control_type *types;
  int count;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->service_app.data_controls.count > 0);
  data_control = &app->service_app.data_controls.data_controls[app->service_app.data_controls.count - 1];

  count = data_control->types.count;
  types = data_control->types.types;
  types = realloc (types, (1 + count) * sizeof *types);
  if (types == NULL)
    return fail_out_of_memory ();

  memset (types + count, 0, sizeof *types);
  data_control->types.types = types;
  data_control->types.count = 1 + count;

  return set_optional_attribute (reader, name, attrs, manif, &types[count].access, "Access");
}

CHA (DataControlType)
{
  struct tpk_app *app;
  struct tpk_data_control *data_control;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->service_app.data_controls.count > 0);
  data_control = &app->service_app.data_controls.data_controls[app->service_app.data_controls.count - 1];
  assert (data_control->types.count > 0);

  return set_simple_string_once (reader, ch, len, manif,
				 &data_control->types.types[data_control->types.count - 1].value, "DataControlType");
}


/*================ Handling AppWidgets =====================*/

static struct xml_read_elem *children_of_app_widgets[] = {
  &xre_AppWidget
};

static struct xml_read_elem *children_of_app_widget[] = {
  &xre_widget_DisplayNames,
  &xre_Sizes,
  &xre_ConfigurationAppControlAppId
};

static struct xml_read_elem *children_of_sizes[] = {
  &xre_Size
};

BEG (AppWidgets)
{
  return xml_read_accept_push (reader, children_of_app_widgets,
			       sizeof children_of_app_widgets / sizeof *children_of_app_widgets);
}

BEG (AppWidget)
{
  struct tpk_app *app;
  int count;
  struct tpk_app_widget *app_widgets;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  count = app->service_app.app_widgets.count;
  app_widgets = app->service_app.app_widgets.app_widgets;
  app_widgets = realloc (app_widgets, (1 + count) * sizeof *app_widgets);
  if (app_widgets == NULL)
    return fail_out_of_memory ();

  memset (app_widgets + count, 0, sizeof *app_widgets);
  app->service_app.app_widgets.app_widgets = app_widgets;
  app->service_app.app_widgets.count = 1 + count;

  if (set_optional_attribute
      (reader, name, attrs, manif,
       &app_widgets[count].app_widget_popup_enable, "AppWidgetPopupEnabled")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app_widgets[count].provider_name,
				 "ProviderName")
      || set_optional_attribute (reader, name, attrs, manif,
				 &app_widgets[count].update_period,
				 "UpdatePeriod")
      || set_optional_attribute (reader, name, attrs, manif, &app_widgets[count].default_, "Default"))
    return -1;

  return xml_read_accept_push (reader, children_of_app_widget,
			       sizeof children_of_app_widget / sizeof *children_of_app_widget);
}

BEG (Sizes)
{
  return xml_read_accept_push (reader, children_of_sizes, sizeof children_of_sizes / sizeof *children_of_sizes);
}

BEG (Size)
{
  struct tpk_app *app;
  struct tpk_app_widget *app_widget;
  struct tpk_size *sizes;
  int count;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->service_app.app_widgets.count > 0);
  app_widget = &app->service_app.app_widgets.app_widgets[app->service_app.app_widgets.count - 1];
  count = app_widget->sizes.count;
  sizes = app_widget->sizes.sizes;

  sizes = realloc (sizes, (1 + count) * sizeof *sizes);
  if (sizes == NULL)
    return fail_out_of_memory ();

  memset (sizes + count, 0, sizeof *sizes);
  app_widget->sizes.sizes = sizes;
  app_widget->sizes.count = 1 + count;

  if (set_optional_attribute
      (reader, name, attrs, manif, &sizes[count].preview_image,
       "PreviewImage")
      || set_optional_attribute (reader, name, attrs, manif, &sizes[count].use_decoration_frame, "UseDecorationFrame"))
    return -1;
  return 0;
}

CHA (Size)
{
  struct tpk_app *app;
  struct tpk_app_widget *app_widget;
  struct tpk_size *size;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->service_app.app_widgets.count > 0);
  app_widget = &app->service_app.app_widgets.app_widgets[app->service_app.app_widgets.count - 1];
  assert (app_widget->sizes.count > 0);
  size = &app_widget->sizes.sizes[app_widget->sizes.count - 1];

  return set_simple_string_once (reader, ch, len, manif, &size->value, "Size");
}

CHA (ConfigurationAppControlAppId)
{
  struct tpk_app *app;
  struct tpk_app_widget *app_widget;

  assert (manif->apps.count > 0);
  app = &manif->apps.apps[manif->apps.count - 1];
  assert (app->service_app.app_widgets.count > 0);
  app_widget = &app->service_app.app_widgets.app_widgets[app->service_app.app_widgets.count - 1];

  return set_simple_string_once (reader, ch, len, manif,
				 &app_widget->configuration_app_control_app_id, "ConfigurationAppControlAppId");
}
