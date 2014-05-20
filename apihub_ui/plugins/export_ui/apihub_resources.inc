<?php
/**
 * @file
 * CTools Export UI plugin for API Hub Resources.
 */

/**
 * CTools Export UI required function for plugin definition
 */
function apihub_ui_apihub_resources_ctools_export_ui() {
  return array(
    'schema'                => 'apihub_resources',
    'access'                => 'administer apihub',
    'menu'                  => array(
      'menu prefix'      => 'admin/structure/apihub/%',
      'menu item'        => 'resources',
      'menu title'       => 'Resources',
      'menu description' => 'Administer 3rd party API Resources.',
    ),
    'strings'               => array(
      'title' => array(
        'edit' => t('Edit %api resource %method %path'),
      ),
    ),
    'title singular'        => t('resource'),
    'title singular proper' => t('Resource'),
    'title plural'          => t('resources'),
    'title plural proper'   => t('Resources'),
    'handler'               => 'apihub_resources_ui',
    'export'                => array(
      'admin_title'       => 'label',
      'admin_description' => 'description',
    ),
  );
}
