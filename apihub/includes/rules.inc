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
  foreach ($handlers as $handler_id => $handler) {
    $handler['id'] = "apihub_handler_{$handler_id}";

    $actions[$handler['id']] = array(
      'label'    => t('Create API Hub "@name" handler', array('@name' => $handler['name'])),
      'group'    => t('API Hub - Handlers'),
      'base'     => 'apihub_rules_handlers_action',
      'provides' => array(
        'handler' => array(
          'label' => t('@name handler', array('@name' => $handler['name'])),
          'type'  => 'apihub_handler',
        ),
      ),
    );

    if (isset($handler['settings']) && is_array($handler['settings'])) {
      $actions[$handler['id']]['parameter'] = array();
      foreach ($handler['settings'] as $setting_id => $setting) {
        $setting['id'] = $setting_id;

        $actions[$handler['id']]['parameter'][$setting['id']] = _apihub_field_to_rules($setting);
      }
    }
  }

  // Add actions from API Hub resources.
  module_load_include('inc', 'ctools', 'includes/export');
  $apis      = ctools_export_crud_load_all('apihub_apis');
  $resources = ctools_export_crud_load_all('apihub_resources');

  foreach ($resources as $resource) {
    $action_id = "apihub_action_{$resource->name}";

    $actions[$action_id] = array(
      'label'     => t('@label (@method @path)', array(
        '@label'  => $resource->label,
        '@method' => $resource->method,
        '@path'   => $resource->path,
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
          'type'  => 'unknown',
        ),
      ),
      'group'     => t('API Hub - Resources - @api', array('@api' => $apis[$resource->api]->label)),
      'base'      => 'apihub_rules_resources_action',
    );

    // Parameters.
    if (!empty($resource->parameters)) {
      foreach ($resource->parameters as $parameter) {
        $actions[$action_id]['parameter'][$parameter['id']] = array(
          'label'         => $parameter['name'],
          'description'   => $parameter['description'],
          'type'          => 'text',
          'default value' => '',
          'optional'      => !$parameter['required'],
        );
        if (!empty($parameter['options'])) {
          $actions[$action_id]['parameter'][$parameter['id']]['options list'] = 'apihub_rules_resources_parameter_options';
        }
      }
    }
  }

  return $actions;
}

/**
 * Implements hook_rules_data_info().
 */
function apihub_rules_data_info() {
  $data['apihub_handler'] = array(
    'label' => t('API Hub handler'),
    'group' => t('API Hub'),
    //    'wrap'          => TRUE,
    //    'property info' => array(
    //
    //    ),
  );

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
  $resource = ctools_export_crud_load('apihub_resources', substr($element->getElementName(), 14));

  if ($resource) {
    $handler = array_shift($args);

    $parameters = array();
    // @TODO - Add some conditional checks to ensure valid settings.
    if (isset($resource->parameters) && is_array($resource->parameters)) {
      foreach ($resource->parameters as $parameter) {
        $parameters[$parameter['id']] = array_shift($args);
      }
    }

    // Process the request.
    $request = new apihub_request($resource->api, $handler['handler'], $handler['settings']);
    $result  = $request->execute($resource->method, $resource->path, $parameters);

    return array(
      'result' => $result,
    );
  }

  return FALSE;
}
