<?php
/**
 * @file
 * Views module integration.
 */

/**
 * Implements hook_views_api().
 */
function apihub_views_api() {
  return array(
    'api' => 3,
  );
}

/**
 * Implements hook_views_plugins().
 */
function apihub_views_plugins() {
  return array(
    'query' => array(
      'apihub_views_plugin_query' => array(
        'title'   => t('API Hub'),
        'help'    => t('API Hub'),
        'handler' => 'apihub_views_plugin_query',
        'path'    => drupal_get_path('module', 'apihub') . '/includes/views'
      ),
    ),
  );
}

/**
 *Implements hook_views_data().
 */
function apihub_views_data() {
  $data = array();

  $handlers = array(
    'field' => array(
      'boolean' => 'views_handler_field_boolean',
      'text'    => 'views_handler_field',
    ),
  );

  ctools_include('export');
  $apis = ctools_export_crud_load_all('apihub_apis');
  foreach ($apis as $api) {
    if (!isset($api->disabled) || !$api->disabled) {
      $resources = ctools_export_load_object('apihub_resources', 'conditions', array('api' => $api->name));

      foreach ($resources as $resource) {
        if (strtoupper($resource->method) == 'GET' && isset($resource->settings['views']) && $resource->settings['views']['enabled']) {
          $data["apihub_{$resource->name}"]['table']['group'] = t('API Hub - @api', array('@api' => $api->admin_title));
          $data["apihub_{$resource->name}"]['table']['base']  = array(
            'title'       => t('API Hub - @api: @admin_title', array(
              '@api'         => $api->admin_title,
              '@admin_title' => $resource->admin_title,
            )),
            'help'        => t(''),
            'query class' => 'apihub_views_plugin_query',
          );

          foreach ($resource->results as $rid => $result) {
            $data["apihub_{$resource->name}"][$result['name']] = array(
              'title'       => $result['name'],
              'description' => $result['description'],
            );

            // Field.
            if (isset($handlers['field'][$result['type']])) {
              $data["apihub_{$resource->name}"][$result['name']]['field'] = array(
                'handler'     => $handlers['field'][$result['type']],
                'description' => $result['description'],
              );
            }
          }
        }
      }
    }
  }

  return $data;
}
