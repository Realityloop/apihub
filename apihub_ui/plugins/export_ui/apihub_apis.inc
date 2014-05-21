<?php
/**
 * @file
 * CTools Export UI plugin for API Hub APIs.
 */

/**
 * CTools Export UI required function for plugin definition
 */
function apihub_ui_apihub_apis_ctools_export_ui() {
  return array(
    'schema'                => 'apihub_apis',
    'access'                => 'administer apihub',
    'menu'                  => array(
      'menu prefix'      => 'admin/structure',
      'menu item'        => 'apihub',
      'menu title'       => 'APIs',
      'menu description' => 'Administer 3rd party APIs.',
      'items'            => array(
        'resources' => array(
          'path' => '%ctools_export_ui/resources',
        ),
      ),
    ),
    'redirect' => array(
      'add' => 'admin/structure/apihub/%ctools_export_ui/resources',
    ),
    'title singular'        => t('api'),
    'title singular proper' => t('API'),
    'title plural'          => t('apis'),
    'title plural proper'   => t('APIs'),
    'handler'               => 'apihub_apis_ui',
    'allowed operations'    => array(
      'resources' => array(
        'title' => 'Resources',
      ),
    ),
    'export'                => array(
      'admin_description' => 'description',
    ),
  );
}
