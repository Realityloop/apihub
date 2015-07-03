<?php
/**
 * @file
 * CTools Export UI plugin for API Hub Methods.
 */

/**
 * CTools Export UI required function for plugin definition
 */
function apihub_ui_apihub_methods_ctools_export_ui() {
  return array(
    'schema'                 => 'apihub_methods',
    'access'                 => 'administer apihub',
    'handler'                => 'apihub_methods_ui',
    'allowed operations'     => array(
      'test'                 => array(
        'title'              => 'Test',
      ),
    ),

    // Menu.
    'menu'                   => array(
      'menu prefix'          => 'admin/structure/apihub/list/%',
      'menu item'            => 'methods',
      'menu title'           => 'Methods',
      'menu description'     => 'Administer 3rd party API Methods.',

      'items'                => array(
        'field'              => array(
          'path'             => 'list/%/field',
          'title'            => 'Test',
          'page callback'    => 'ctools_export_ui_switcher_page',
          'page arguments'   => array('apihub_methods', 'field', 7, 9, 10),
          'load arguments'   => array('apihub_methods'),
          'access callback'  => TRUE,
          'type'             => MENU_CALLBACK,
        ),
        'list callback'      => array(
          'title'            => 'Methods',
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
          'page arguments'   => array('apihub_methods', 'test', 7),
          'load arguments'   => array('apihub_methods'),
          'access callback'  => 'ctools_export_ui_task_access',
          'access arguments' => array('apihub_methods', 'test', 7),
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
        'edit'               => t('Edit %api method %method %path'),
        'test'               => t('Test %api method %method %path'),
      ),
    ),
    'title singular'         => t('method'),
    'title singular proper'  => t('Method'),
    'title plural'           => t('methods'),
    'title plural proper'    => t('Methods'),
  );
}
