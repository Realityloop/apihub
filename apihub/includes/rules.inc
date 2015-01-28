<?php
/**
 * @file
 * Rules module integration.
 */

/**
 * Implements hook_rules_action_info().
 */
function apihub_rules_action_info() {
  $actions = array();
//
//  // Add actions from API Hub handlers.
//  $handlers = apihub_handlers();
//  foreach ($handlers as $handler_id => $handler) {
//    $handler['id'] = "apihub_handler_{$handler_id}";
//
//    $actions[$handler['id']] = array(
//      'label'    => t('Create API Hub "@name" handler', array('@name' => $handler['name'])),
//      'group'    => t('API Hub - Handlers'),
//      'base'     => 'apihub_rules_handlers_action',
//      'provides' => array(
//        'handler' => array(
//          'label' => t('@name handler', array('@name' => $handler['name'])),
//          'type'  => 'apihub_handler',
//        ),
//      ),
//    );
//
//    if (isset($handler['settings']) && is_array($handler['settings'])) {
//      $actions[$handler['id']]['parameter'] = array();
//      foreach ($handler['settings'] as $setting_id => $setting) {
//        $setting['id'] = $setting_id;
//
//        $actions[$handler['id']]['parameter'][$setting['id']] = _apihub_field_to_rules($setting);
//      }
//    }
//  }
//
//  // Add actions from API Hub resources.
//  ctools_include('export');
//  $apis = ctools_export_crud_load_all('apihub_apis');
//  foreach ($apis as $api) {
//    if (!isset($api->disabled) || !$api->disabled) {
//      $resources = ctools_export_load_object('apihub_resources', 'conditions', array('api' => $api->name));
//
//      foreach ($resources as $resource) {
//        $action_id = "apihub_action_{$resource->name}";
//
//        $actions[$action_id] = array(
//          'label'     => t('@label (@method @path)', array(
//            '@label'  => $resource->admin_title,
//            '@method' => $resource->method,
//            '@path'   => $resource->path,
//          )),
//          'parameter' => array(
//            'handler' => array(
//              'label' => t('Handler'),
//              'type'  => 'apihub_handler',
//            ),
//          ),
//          'provides'  => array(
//            'result' => array(
//              'label' => t('Result'),
//              'type'  => "apihub_data_{$resource->name}",
//            ),
//          ),
//          'group'     => t('API Hub - Resources - @api', array('@api' => $apis[$resource->api]->admin_title)),
//          'base'      => 'apihub_rules_resources_action',
//        );
//
//        // Parameters.
//        if (!empty($resource->parameters)) {
//          foreach ($resource->parameters as $parameter) {
//            $actions[$action_id]['parameter'][$parameter['name']] = array(
//              'label'         => $parameter['name'],
//              'description'   => $parameter['description'],
//              'type'          => 'text',
//              'default value' => '',
//              'optional'      => !$parameter['required'],
//            );
//            if (!empty($parameter['options'])) {
//              $actions[$action_id]['parameter'][$parameter['id']]['options list'] = 'apihub_rules_resources_parameter_options';
//            }
//          }
//        }
//
//        // Provides - <list>.
//        // @TODO - Deal with 'root' settings here as well?
//        $root = array();
//        foreach ($resource->results as $rid => $result) {
//          if (empty($result['pid'])) {
//            $root[] = $rid;
//          }
//        }
//        if (count($root) == 1 && $resource->results[$root[0]]['type'] == 'group') {
//          $actions[$action_id]['provides']['result']['type'] = "list<apihub_data_{$resource->name}>";
//        }
//      }
//    }
//  }

  return $actions;
}




/**
 * Implements hook_rules_data_info().
 */
function apihub_rules_data_info() {
  $data['apihub_handler'] = array(
    'label'    => t('API Hub handler'),
    'group'    => t('API Hub'),
    'ui class' => 'APIHubRulesDataUIHandler',
  );

  // Add data types from API Hub resources.
  ctools_include('export');
  $apis = ctools_export_crud_load_all('apihub_apis');
  foreach ($apis as $api) {
    if (!isset($api->disabled) || !$api->disabled) {
      $resources = ctools_export_load_object('apihub_resources', 'conditions', array('api' => $api->name));

      foreach ($resources as $resource) {
        // Reset the root if only root element is a group.
        $root = array();
        foreach ($resource->results as $rid => $result) {
          if (empty($result['pid'])) {
            $root[] = $rid;
          }
        }
        if (count($root) == 1 && $resource->results[$root[0]]['type'] == 'group') {
          unset($resource->results[$root[0]]);
          foreach ($resource->results as $rid => &$result) {
            if ($result['pid'] == $root[0]) {
              $result['pid'] = '';
            }
          }
        }

        $data_id = "apihub_data_{$resource->name}";

        $data[$data_id] = array(
          'label'         => t('@label (@method @path) result', array(
            '@label'  => $resource->admin_title,
            '@method' => $resource->method,
            '@path'   => $resource->path,
          )),
          'group'         => t('API Hub - @api', array('@api' => $apis[$resource->api]->admin_title)),
          'wrap'          => TRUE,
          'property info' => array(),
        );

        $map = array();
        foreach ($resource->results as $rid => $result) {
          $map[$rid] = & $data[$data_id]['property info'];
          if (!empty($result['pid'])) {
            $parent = $result['pid'];
            while (isset($parent)) {
              $map[$rid] = & $map[$parent];

              if (!empty($resource->results[$parent]['pid'])) {
                $parent = $resource->results[$parent]['pid'];
              }
              else {
                unset($parent);
              }
            }
          }

          $key = empty($result['name']) ? count($map[$rid]) : $result['name'];
          switch ($result['type']) {
            case 'group':
              $map[$rid][$key] = array(
                'label'         => $result['name'],
                'property info' => array(),
                'type'          => 'list',
              );
              $map[$rid]       = & $map[$rid][$key]['property info'];
              break;

            default:
              $map[$rid][$key] = array(
                'label' => $result['name'],
                'id'    => $key,
                'type'  => $result['type'],
              );
              $map[$rid]       = & $map[$rid][$key];
              break;
          }
        }
      }
    }
  }

  return $data;
}

/**
 * Convert an API Hub field to a Rules element.
 *
 * @param $field
 *
 * @return array
 */
function _apihub_field_to_rules($field) {
  $field += array(
    'name'        => '',
    'type'        => 'text',
    'description' => '',
    'required'    => FALSE,
  );

  // @TODO - Make this extensible.
  $types = array(
    'decimal' => 'decimal',
    'text'    => 'text',
    'user'    => 'user',
  );

  $element = array(
    'label'       => $field['name'],
    'type'        => isset($types[$field['type']]) ? $types[$field['type']] : 'textfield',
    'description' => $field['description'],
    'optional'    => !$field['required'],
  );

  if (isset($field['options callback']) && function_exists($field['options callback'])) {
    $element['options list'] = $field['options callback'];
  }

  return $element;
}

/**
 * Handlers action callback.
 *
 * @return array|bool
 */
function apihub_rules_handlers_action() {
  $args = func_get_args();

  $element = $args[count($args) - 2];
  $handler = apihub_handlers_load(substr($element->getElementName(), 15));

  if ($handler) {
    $return = array(
      'handler'  => $handler['id'],
      'settings' => array(),
    );

    // @TODO - Add some conditional checks to ensure valid settings.
    if (isset($handler['settings']) && is_array($handler['settings'])) {
      foreach (array_keys($handler['settings']) as $setting) {
        $return['settings'][$setting] = array_shift($args);
      }
    }

    return array(
      'handler' => $return,
    );
  }

  // @TODO - Throw an exception.
  return FALSE;
}

/**
 * Generate options list based on parameters available options.
 *
 * @param $element
 * @param $name
 *
 * @return array
 */
function apihub_rules_resources_parameter_options($element, $name) {
  ctools_include('export');
  $resource = ctools_export_crud_load('apihub_resources', substr($element->getElementName(), 14));
  foreach ($resource->parameters as $_parameter) {
    if ($_parameter['id'] == $name) {
      $parameter = $_parameter;
    }
  }

  $options = array('' => t('- Default -'));
  $options += list_extract_allowed_values($parameter['options'], 'list_text', FALSE);

  return $options;
}

/**
 *
 */
function apihub_rules_resources_action() {
  $args = func_get_args();

  // Load provided API Hub resource.
  $element = $args[count($args) - 2];
  ctools_include('export');
  $resource = ctools_export_crud_load('apihub_resources', substr($element->getElementName(), 14));

  if ($resource) {
    $handler = array_shift($args);

    $parameters = array();
    // @TODO - Add some conditional checks to ensure valid settings.
    if (isset($resource->parameters) && is_array($resource->parameters)) {
      foreach ($resource->parameters as $parameter) {
        $parameters[$parameter['name']] = array_shift($args);
      }
    }

    // Process the request.
    $request = new apihubRequest($resource->api, $handler['handler'], $handler['settings']);
    $result  = $request->execute($resource->method, $resource->path, $parameters);

    // Return resulting data.
    $return = array(
      'result' => $result,
    );

    if (!empty($resource->settings['rules']['provides']) && isset($result->{$resource->settings['rules']['provides']['id']})) {
      $return[$resource->settings['rules']['provides']['id']] = $result->{$resource->settings['rules']['provides']['id']};
    }

    return $return;
  }

  return FALSE;
}
