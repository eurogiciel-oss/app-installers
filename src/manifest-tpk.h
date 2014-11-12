/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#pragma once

struct locstr
{
  char *locale;
  char *value;
};

struct named
{
  char *name;
  char *value;
};

struct icon
{
  char *section;
  char *name;
};

struct resolution
{
  char *mime_type;
  char *uri_scheme;
};

struct capability
{
  char *operation_id;
  struct
  {
    int count;
    struct resolution *resolutions;
  } resolutions;
};

struct app_control
{
  char *provider_id;
  struct
  {
    int count;
    struct capability *capabilities;
  } capabilities;
};

struct data_control_type
{
  char *access;
  char *value;
};

struct data_control
{
  char *provider_id;
  struct
  {
    int count;
    struct data_control_type *types;
  } types;
};

struct display_names
{
  int count;
  struct locstr *display_names;
};

struct icons
{
  int count;
  struct icon *icons;
};

enum app_type
{ unset_app, ui_app, service_app };

struct app
{
  enum app_type type;
  char *name;
  char *main;
  char *menu_icon_visible;
  char *launching_history_visible;
  char *category;
  char *hw_acceleration;
  char *sub_mode;
  char *launch_on_boot;
  char *auto_restart;
  char *use_ui;
  struct display_names display_names;
  struct icons icons;
  struct
  {
    int count;
    struct named *conditions;
  } launch_conditions;
  struct
  {
    int count;
    struct named *notifications;
  } notifications;
  struct
  {
    int count;
    struct app_control *app_controls;
  } app_controls;

  struct
  {
    char *coordinate_system;
    char *base_screen_size;
    char *logical_coordinate;
  } ui_scalability;
  struct
  {
    char *system_theme;
    char *user_defined_theme;
  } ui_theme;
  struct
  {
    char *uuid;
    struct
    {
      int count;
      char **languages;
    } languages;
  } ime;

  struct
  {
    int count;
    struct data_control *data_controls;
  } data_controls;
};

struct manifest_tpk
{
  char *locale;
  char *id;
  char *version;
  char *installation_location;
  char *type;
  char *author;
  char *url;
  char *api_version;
  struct
  {
    int count;
    struct locstr *descriptions;
  } descriptions;
  struct
  {
    int count;
    struct named *features;
  } features;
  struct
  {
    int count;
    char **privileges;
  } privileges;
  struct
  {
    int count;
    struct app *apps;
  } apps;
};

#if !defined(NDEBUG)
void manifest_tpk_dump (struct manifest_tpk *manif);
#endif

int manifest_tpk_from_xml_file (struct manifest_tpk **manif,
				const char *path);
