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
    'schema'                 => 'apihub_resources',
    'access'                 => 'administer apihub',
    'handler'                => 'apihub_resources_ui',
    'allowed operations'     => array(
      'test'                 => array(
        'title'              => 'Test',
      ),
    ),

    // Menu.
    'menu'                   => array(
      'menu prefix'          => 'admin/structure/apihub/list/%',
      'menu item'            => 'resources',
      'menu title'           => 'Resources',
      'menu description'     => 'Administer 3rd party API Resources.',

      'items'                => array(
        'field'              => array(
          'path'             => 'list/%ctools_export_ui/field',
          'title'            => 'Test',
          'page callback'    => 'ctools_export_ui_switcher_page',
          'page arguments'   => array('apihub_resources', 'field', 7, 9, 10),
          'load arguments'   => array('apihub_resources'),
          'access callback'  => TRUE,
          'type'             => MENU_CALLBACK,
        ),
        'list callback'      => array(
          'title'            => 'Resources',
          'type'             => MENU_LOCAL_TASK,
          'tab_root'         => 'admin/structure/apihub/list/%',
          'weight'           => -10,
        ),
        'list'               => array(
          'type'             => MENU_NORMAL_ITEM,
        ),
        'test'               => array(
          'path'             => 'list/%ctools_export_ui/test',
          'title'            => 'Test',
          'page callback'    => 'ctools_export_ui_switcher_page',
          'page arguments'   => array('apihub_resources', 'test', 7),
          'load arguments'   => array('apihub_resources'),
          'access callback'  => 'ctools_export_ui_task_access',
          'access arguments' => array('apihub_resources', 'test', 7),
          'type'             => MENU_LOCAL_TASK,
          'weight'           => -10,
        ),
      ),
    ),

    // Strings.
    'strings'                => array(
      'confirmation'         => array(
        'edit'               => array(
          'success'          => t('%title has been updated.'),
          'fail'             => t('%title could not be updated.'),
        ),
      ),
      'title'                => array(
        'edit'               => t('Edit %api resource %method %path'),
        'test'               => t('Test %api resource %method %path'),
      ),
    ),
    'title singular'         => t('resource'),
    'title singular proper'  => t('Resource'),
    'title plural'           => t('resources'),
    'title plural proper'    => t('Resources'),
  );
}
