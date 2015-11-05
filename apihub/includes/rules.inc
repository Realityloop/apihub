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

  // Add actions from API Hub handlers.
  $handlers = apihub_handlers();
  foreach ($handlers as $class => $name) {
    if (class_exists($class)) {
      $id = "apihub_handler_{$class}";

      $actions[$id] = array(
        'label'    => t('Create API Hub "@name" handler', array('@name' => $name)),
        'group'    => t('API Hub - Handlers'),
        'base'     => 'apihub_rules_handlers_action',
        'provides' => array(
          'handler' => array(
            'label' => t('@name handler', array('@name' => $name)),
            'type'  => 'apihub_handler',
          ),
        ),
      );

      $handler = new $class();
      if (method_exists($handler, 'settingsForm')) {
        $actions[$id]['parameter'] = array();
        foreach ($handler->settingsForm() as $setting => $element) {
          $actions[$id]['parameter'][$setting] = _apihub_field_to_rules($element);
        }
      }
    }
  }

  // Add actions from API Hub method.
  ctools_include('export');
  $apis = ctools_export_crud_load_all('apihub_apis');
  foreach ($apis as $api) {
    if (!isset($api->disabled) || !$api->disabled) {
      $methods = ctools_export_load_object('apihub_methods', 'conditions', array('api' => $api->name));

      foreach ($methods as $method) {
        $action_id = "apihub_action_{$method->name}";

        $actions[$action_id] = array(
          'label'     => t('@label (@http_method @path)', array(
            '@label'       => $method->admin_title,
            '@http_method' => $method->http_method,
            '@path'        => $method->path,
          )),
          'parameter' => array(
            'handler' => array(
              'label' => t('Handler'),
              'type'  => 'apihub_handler',
            ),
          ),
          'provides'  => array(
            'result' => array(
              'label' => t('Result'),
              'type'  => "apihub_data_{$method->name}",
            ),
          ),
          'group'     => t('API Hub - Methods - @api', array('@api' => $method->api->admin_title)),
          'base'      => 'apihub_rules_methods_action',
        );

        // Parameters.
        if (!empty($method->parameters)) {
          foreach ($method->parameters as $parameter) {
            $actions[$action_id]['parameter'][$parameter['name']] = _apihub_field_to_rules($parameter);
            if (!empty($parameter['options'])) {
              $actions[$action_id]['parameter'][$parameter['id']]['options list'] = 'apihub_rules_methods_parameter_options';
            }
          }
        }

        // Provides - <list>.
        // @TODO - Deal with 'root' settings here as well?
        $root = array();
        foreach ($method->results as $rid => $result) {
          if (empty($result['pid'])) {
            $root[] = $rid;
          }
        }
        if (count($root) == 1 && $method->results[$root[0]]['type'] == 'group') {
          $actions[$action_id]['provides']['result']['type'] = "list<apihub_data_{$method->name}>";
        }
      }
    }
  }

  return $actions;
}

/**
 * Implements hook_rules_condition_info().
 */
function apihub_rules_condition_info() {
  $conditions['apihub_condition_isset'] = array(
    'label'     => t('Isset'),
    'group'     => t('API Hub'),
    'parameter' => array(
      'method'  => array(
        'type'         => 'text',
        'label'        => t('Method'),
        'description'  => t(''),
        'options list' => '_apihub_rules_condition_methods_list',
      ),
      'handler' => array(
        'type'        => 'apihub_handler',
        'label'       => t('Handler'),
        'description' => t(''),
      ),
    ),
  );

  return $conditions;
}

function _apihub_rules_condition_methods_list() {
  return array('test' => t('Test'));
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

  // Add data types from API Hub methods.
  ctools_include('export');
  $apis = ctools_export_crud_load_all('apihub_apis');
  foreach ($apis as $api) {
    if (!isset($api->disabled) || !$api->disabled) {
      $methods = ctools_export_load_object('apihub_methods', 'conditions', array('api' => $api->name));

      foreach ($methods as $method) {
        // Reset the root if only root element is a group.
        $root = array();
        foreach ($method->results as $rid => $result) {
          if (empty($result['pid'])) {
            $root[] = $rid;
          }
        }
        if (count($root) == 1 && $method->results[$root[0]]['type'] == 'group') {
          unset($method->results[$root[0]]);
          foreach ($method->results as $rid => &$result) {
            if ($result['pid'] == $root[0]) {
              $result['pid'] = '';
            }
          }
        }

        $data_id = "apihub_data_{$method->name}";

        $data[$data_id] = array(
          'label'         => t('@label (@http_method @path) result', array(
            '@label'       => $method->admin_title,
            '@http_method' => $method->http_method,
            '@path'        => $method->path,
          )),
          'group'         => t('API Hub - @api', array('@api' => $method->api->admin_title)),
          'wrap'          => TRUE,
          'property info' => array(),
        );

        $map = array();
        foreach ($method->results as $rid => $result) {
          $map[$rid] = &$data[$data_id]['property info'];
          if (!empty($result['pid'])) {
            $parent = $result['pid'];
            while (isset($parent)) {
              $map[$rid] = &$map[$parent];

              if (!empty($method->results[$parent]['pid'])) {
                $parent = $method->results[$parent]['pid'];
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
              $map[$rid] = &$map[$rid][$key]['property info'];
              break;

            default:
              $map[$rid][$key] = array(
                'label' => $result['name'],
                'id'    => $key,
                'type'  => $result['type'],
              );
              $map[$rid] = &$map[$rid][$key];
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
    'boolean' => 'boolean',
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
  $class = substr($element->getElementName(), 15);
  $handler = apihub_handlers_load($class);
  $settings = array();
  if ($handler) {
    // @TODO - Add some conditional checks to ensure valid settings.
    if (method_exists($handler, 'settingsForm')) {
      foreach ($handler->settingsForm() as $setting => $element) {
        $settings[$setting] = array_shift($args);
      }
    }

    return array(
      'handler' => array(
        'class'    => $class,
        'settings' => $settings,
      ),
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
function apihub_rules_methods_parameter_options($element, $name) {
  ctools_include('export');
  $method = ctools_export_crud_load('apihub_methods', substr($element->getElementName(), 14));
  foreach ($method->parameters as $_parameter) {
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
function apihub_rules_methods_action() {
  $args = func_get_args();

  // Load provided API Hub method.
  /** @var RulesAction $element */
  $element = $args[count($args) - 2];

  ctools_include('export');
  /** @var apihubMethod $method */
  $method = ctools_export_crud_load('apihub_methods', substr($element->getElementName(), 14));
  if ($method) {
    $handler = array_shift($args);

    $parameters = array();
    // @TODO - Add some conditional checks to ensure valid settings.
    if (isset($method->parameters) && is_array($method->parameters)) {
      foreach ($method->parameters as $parameter) {
        $parameters[$parameter['name']] = array_shift($args);
      }
    }

    // Process the request.
    $result = $method->execute($parameters, $handler);

    // Return resulting data.
    $return = array(
      'result' => $result,
    );

    if (!empty($method->settings['rules']['provides']) && isset($result->{$method->settings['rules']['provides']['id']})) {
      $return[$method->settings['rules']['provides']['id']] = $result->{$method->settings['rules']['provides']['id']};
    }

    return $return;
  }

  return FALSE;
}
