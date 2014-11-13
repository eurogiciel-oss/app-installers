/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#pragma once

struct tpk_locstr
{
  char *locale;
  char *value;
};

struct tpk_named
{
  char *name;
  char *value;
};

struct tpk_icon
{
  char *section;
  char *name;
};

struct tpk_resolution
{
  char *mime_type;
  char *uri_scheme;
};

struct tpk_app_capability
{
  char *operation_id;
  struct
  {
    int count;
    struct tpk_resolution *resolutions;
  } resolutions;
};

struct tpk_app_control
{
  char *provider_id;
  struct
  {
    int count;
    struct tpk_app_capability *capabilities;
  } capabilities;
};

struct tpk_display_names
{
  int count;
  struct tpk_locstr *display_names;
};

struct tpk_icons
{
  int count;
  struct tpk_icon *icons;
};

struct tpk_launch_conditions
{
  int count;
  struct tpk_named *conditions;
};

struct tpk_notifications
{
  int count;
  struct tpk_named *notifications;
};

struct tpk_app_controls
{
  int count;
  struct tpk_app_control *app_controls;
};

struct tpk_ui_scalability
{
  char *coordinate_system;
  char *base_screen_size;
  char *logical_coordinate;
};

struct tpk_ui_theme
{
  char *system_theme;
  char *user_defined_theme;
};

struct tpk_languages
{
  int count;
  char **languages;
};

struct tpk_ime
{
  char *uuid;
  struct tpk_languages languages;
};

struct tpk_account_capabilities
{
  int count;
  char **capabilities;
};

struct tpk_account_provider
{
  char *multiple_account_support;
  struct tpk_icons icons;
  struct tpk_display_names display_names;
  struct tpk_account_capabilities capabilities;
};

struct tpk_accounts
{
  int count;
  struct tpk_account_provider *accounts;
};

struct tpk_data_control_type
{
  char *access;
  char *value;
};

struct tpk_data_control
{
  char *provider_id;
  struct
  {
    int count;
    struct tpk_data_control_type *types;
  } types;
};

struct tpk_data_controls
{
  int count;
  struct tpk_data_control *data_controls;
};

struct tpk_size
{
  char *preview_image;
  char *use_decoration_frame;
  char *value;
};

struct tpk_sizes
{
  int count;
  struct tpk_size *sizes;
};

struct tpk_app_widget
{
  char *app_widget_popup_enable;
  char *provider_name;
  char *update_period;
  char *default_;
  char *configuration_app_control_app_id;
  struct tpk_display_names display_names;
  struct tpk_sizes sizes;
};

struct tpk_app_widgets
{
  int count;
  struct tpk_app_widget *app_widgets;
};

enum app_type
{ unset_app, ui_app, service_app };

struct tpk_ui_app
{
  char *menu_icon_visible;
  char *launching_history_visible;
  char *category;
  char *hw_acceleration;
  char *sub_mode;
  struct tpk_ui_scalability ui_scalability;
  struct tpk_ui_theme ui_theme;
  struct tpk_ime ime;
  struct tpk_accounts accounts;
};

struct tpk_service_app
{
  char *launch_on_boot;
  char *auto_restart;
  char *use_ui;
  struct tpk_data_controls data_controls;
  struct tpk_app_widgets app_widgets;
};

struct tpk_app
{
  char *name;
  char *main;

  struct tpk_display_names display_names;
  struct tpk_icons icons;
  struct tpk_launch_conditions launch_conditions;
  struct tpk_notifications notifications;
  struct tpk_app_controls app_controls;

  enum app_type type;
  struct tpk_ui_app ui_app;
  struct tpk_service_app service_app;

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
    struct tpk_locstr *descriptions;
  } descriptions;
  struct
  {
    int count;
    struct tpk_named *features;
  } features;
  struct
  {
    int count;
    char **privileges;
  } privileges;
  struct
  {
    int count;
    struct tpk_app *apps;
  } apps;
};

#if !defined(NDEBUG)
void manifest_tpk_dump (struct manifest_tpk *manif);
#endif

int manifest_tpk_from_xml_file (struct manifest_tpk **manif, const char *path);
