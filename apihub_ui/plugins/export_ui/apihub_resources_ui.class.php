<?php

/**
 * Class apihub_resources_ui
 */
class apihub_resources_ui extends ctools_export_ui {

  /**
   * Builds the operation links for a specific exportable item.
   */
  function build_operations($item) {
    $allowed_operations = parent::build_operations($item);

    foreach ($allowed_operations as &$operation) {
      $operation['href'] = str_replace('%', arg(3), $operation['href']);
    }

    return $allowed_operations;
  }

  /**
   * Provide the actual editing form.
   */
  function edit_form(&$form, &$form_state) {
    parent::edit_form($form, $form_state);

    global $user;

    $export_key = $this->plugin['export']['key'];
    $item       = $form_state['item'];
    $schema     = ctools_export_get_schema($this->plugin['schema']);

    if (empty($form_state['values'])) {
      $api = ctools_export_crud_load('apihub_apis', arg(3));
    }
    else {
      $api = ctools_export_crud_load('apihub_apis', $form_state['values']['api']);
    }

    if ($form_state['op'] !== 'edit') {
      unset($form['info'][$export_key]);
    }

    // Set API name.
    $form['info']['api'] = array(
      '#type'  => 'value',
      '#value' => $api->name,
    );

    $form['info']['method'] = array(
      '#type'          => 'select',
      '#title'         => t('Method'),
      '#options'       => array(
        'GET'     => 'GET',
        'HEAD'    => 'HEAD',
        'POST'    => 'POST',
        'PUT'     => 'PUT',
        'DELETE'  => 'DELETE',
        'TRACE'   => 'TRACE',
        'OPTIONS' => 'OPTIONS',
        'CONNECT' => 'CONNECT',
        'PATCH'   => 'PATCH',
      ),
      '#default_value' => $item->method,
      '#disabled'      => $form_state['op'] === 'edit',
    );

    $form['info']['path'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Path'),
      '#required'      => TRUE,
      '#default_value' => $item->path,
      '#field_prefix'  => $api->url,
      '#disabled'      => $form_state['op'] === 'edit',
    );

    // Parameters.
    // @TODO - Toggle edit mode/summary mode.
    // @TODO - Make this drag'n'drop sortable.
    $form['parameters'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('Parameters'),
      '#tree'        => TRUE,
      '#collapsible' => TRUE,
//      '#collapsed'   => $form_state['op'] === 'edit',
      '#theme'       => 'apihub_ui_method_parameters',
      '#prefix'      => '<div id="parameters-wrapper">',
      '#suffix'      => '</div>',
    );

    if (is_null($item->parameters)) {
      $item->parameters = array();
    }
    if (!empty($form_state['clicked_button']) && $form_state['clicked_button']['#value'] == t('Add another')) {
      $item->parameters[] = array();
    }
    foreach ($item->parameters as $delta => $parameter) {
      $form['parameters'][$delta]['name'] = array(
        '#type'          => 'textfield',
        '#title'         => t('Name'),
        '#title_display' => 'invisible',
        '#size'          => 32,
        '#default_value' => isset($parameter['name']) ? $parameter['name'] : '',
      );

      $form['parameters'][$delta]['id'] = array(
        '#type'          => 'textfield',
        '#title'         => t('ID'),
        '#title_display' => 'invisible',
        '#size'          => 16,
        '#default_value' => isset($parameter['id']) ? $parameter['id'] : '',
      );

      $form['parameters'][$delta]['type'] = array(
        '#type'          => 'select',
        '#title'         => t('Type'),
        '#title_display' => 'invisible',
        '#options'       => array(
          'text'    => t('Text'),
          'boolean' => t('Boolean'),
          'decimal' => t('Number'),
        ),
        '#default_value' => isset($parameter['type']) ? $parameter['type'] : '',
      );

      $form['parameters'][$delta]['options'] = array(
        '#type'          => 'textarea',
        '#title'         => t('Available options'),
        '#title_display' => 'invisible',
        '#rows'          => 3,
        '#default_value' => isset($parameter['options']) ? $parameter['options'] : '',
        '#states'        => array(
          'visible' => array(
            ":input[name='parameters[{$delta}][type]']" => array('value' => 'text'),
          ),
        ),
      );

      $form['parameters'][$delta]['required'] = array(
        '#type'          => 'checkbox',
        '#title'         => t('Required'),
        '#title_display' => 'invisible',
        '#default_value' => isset($parameter['required']) ? $parameter['required'] : FALSE,
      );

      $form['parameters'][$delta]['description'] = array(
        '#type'          => 'textarea',
        '#title'         => t('Description'),
        '#rows'          => 2,
        '#default_value' => isset($parameter['description']) ? $parameter['description'] : '',
      );
    }
    $form['parameters']['_add'] = array(
      '#type'  => 'button',
      '#value' => t('Add another'),
      '#ajax'  => array(
        'callback' => 'apihub_ui_resources_form_js_add',
        'wrapper'  => 'parameters-wrapper',
      ),
    );

    // Test.
    $form['test'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('Test'),
      '#tree'        => TRUE,
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
    );

    // Handler.
    $handler_cid   = "apihub:{$item->api}:handler:{$user->uid}";
    $handler_cache = cache_get($handler_cid);

    $form['test']['handler'] = array(
      '#type'    => 'select',
      '#title'   => t('Handler'),
      '#options' => apihub_ui_resources_handlers(),
      '#ajax'    => array(
        'callback' => 'apihub_ui_resources_form_js_test_handler_settings',
        'wrapper'  => 'test-handler-settings-output',
      ),
    );

    $form['test']['handler_settings'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('Handler settings'),
      '#collapsible' => TRUE,
      '#prefix'      => '<div id="test-handler-settings-output">',
      '#suffix'      => '</div>',
    );

    // Determine which handler is currently in use.
    $handler = key($form['test']['handler']['#options']);
    if (isset($handler_cache->data['handler']) && in_array($handler_cache->data['handler'], array_keys($form['test']['handler']['#options']))) {
      $handler                                   = $handler_cache->data['handler'];
      $form['test']['handler']['#default_value'] = $form['test']['handler'];
    }

    $handler = apihub_handlers_load($handler);
    if ($handler && isset($handler['settings']) && !empty($handler['settings'])) {
      foreach ($handler['settings'] as $id => $setting) {
        $form['test']['handler_settings'][$id] = _apihub_field_to_fapi($setting);
        if (isset($handler_cache->data['settings'][$id])) {
          $form['test']['handler_settings'][$id]['#default_value'] = $handler_cache->data['settings'][$id];
        }
      }
    }

    // Input.
    $input_cid   = "apihub:{$item->api}:input:{$item->name}:{$user->uid}";
    $input_cache = cache_get($input_cid);

    $form['test']['input'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('Input'),
      '#collapsible' => TRUE,
    );

    foreach ($item->parameters as $parameter) {
      if (empty($parameter)) {
        continue;
      }

      $form['test']['input'][$parameter['id']] = _apihub_field_to_fapi($parameter);
      if (isset($input_cache->data[$parameter['id']])) {
        $form['test']['input'][$parameter['id']]['#default_value'] = $input_cache->data[$parameter['id']];
      }
    }
    $inputs = element_children($form['test']['input']);
    if (empty($inputs)) {
      unset($form['test']['input']);
    }

    // Output.
    $form['test']['output'] = array(
      '#type'   => 'item',
      '#title'  => t('Output'),
      '#markup' => t('No data'),
      '#prefix' => '<div id="test-output-wrapper">',
      '#suffix' => '</div>',
    );

    $form['test']['submit'] = array(
      '#type'  => 'button',
      '#value' => $item->method,
      '#ajax'  => array(
        'callback' => 'apihub_ui_resources_form_js_test_output',
        'wrapper'  => 'test-output-wrapper',
      ),
    );
  }

  /**
   * Validate callback for the edit form.
   */
  function edit_form_validate(&$form, &$form_state) {
    parent::edit_form_validate($form, $form_state);

    unset($form_state['values']['parameters']['_add']);
    foreach ($form_state['values']['parameters'] as $delta => $parameter) {
      // Ensure only populated data is saved.
      if (empty($parameter['id'])) {
        unset($form_state['values']['parameters'][$delta]);
      }
    }

    if ($form_state['op'] != 'edit') {
      // Build the export key.
      $export_key                        = $this->plugin['export']['key'];
      $form_state['item']->{$export_key} = md5("{$form_state['values']['api']}::{$form_state['values']['method']}::{$form_state['values']['path']}");
    }
  }

  /**
   * Get a page title for the current page from our plugin strings.
   */
  function get_page_title($op, $item = NULL) {
    $title = parent::get_page_title($op, $item);

    if (!empty($item)) {
      $title = str_replace('%api', check_plain($item->api), $title);
      $title = str_replace('%method', check_plain($item->method), $title);
      $title = str_replace('%path', check_plain($item->path), $title);
    }

    return $title;
  }

  /**
   * Build a row based on the item.
   *
   * By default all of the rows are placed into a table by the render
   * method, so this is building up a row suitable for theme('table').
   * This doesn't have to be true if you override both.
   */
  function list_build_row($item, &$form_state, $operations) {
    parent::list_build_row($item, $form_state, $operations);

    foreach ($this->rows[$item->name]['data'] as $delta => &$col) {
      if (in_array('ctools-export-ui-name', $col['class'])) {
        $col = array(
          'data'  => $item->path,
          'class' => array(
            'ctools-export-ui-path',
          ),
        );
        break;
      }
    }

    $additional_items[] = array(
      'data'  => $item->method,
      'class' => array(
        'ctools-export-ui-method',
      ),
    );

    array_splice($this->rows[$item->name]['data'], $delta + 1, 0, $additional_items);

    if (isset($this->sorts[$item->name])) {
      $this->sorts[$item->name] = $item->admin_title;
    }
  }

  /**
   * Determine if a row should be filtered out.
   *
   * This handles the default filters for the export UI list form. If you
   * added additional filters in list_form() then this is where you should
   * handle them.
   *
   * @return
   *   TRUE if the item should be excluded.
   */
  function list_filter($form_state, $item) {
    if ($item->api != arg(3)) {
      return TRUE;
    }

    return parent::list_filter($form_state, $item);
  }

  /**
   * Master entry point for handling a list.
   *
   * It is unlikely that a child object will need to override this method,
   * unless the listing mechanism is going to be highly specialized.
   */
  function list_page($js, $input) {
    $admin_title = apihub_resources_ui_api('admin_title');
    drupal_set_title(t('@admin_title resources', array('@admin_title' => $admin_title)));

    return parent::list_page($js, $input);
  }

  /**
   * Provide the table header.
   *
   * If you've added columns via list_build_row() but are still using a
   * table, override this method to set up the table header.
   */
  function list_table_header() {
    $header = parent::list_table_header();

    foreach ($header as $delta => &$item) {
      if ($item['data'] == t('Name')) {
        $item['data'] = t('Path');
        break;
      }
    }

    $additional_items[] = array(
      'data'  => t('Method'),
      'class' => array(
        'ctools-export-ui-method',
      ),
    );

    array_splice($header, $delta + 1, 0, $additional_items);

    return $header;
  }

  /**
   * Perform a drupal_goto() to the location provided by the plugin for the
   * operation.
   *
   * @param $op
   *   The operation to use. A string must exist in $this->plugin['redirect']
   *   for this operation.
   * @param $item
   *   The item in use; this may be necessary as item IDs are often embedded in
   *   redirects.
   */
  function redirect($op, $item = NULL) {
    if (isset($this->plugin['redirect'][$op])) {
      $destination = (array) $this->plugin['redirect'][$op];
      if ($item) {
        $destination[0] = str_replace('%', $item->api, $destination[0]);
      }
      call_user_func_array('drupal_goto', $destination);
    }

    parent::redirect($op, $item);
  }

}

/**
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function apihub_ui_resources_form_js_add($form, $form_state) {
  drupal_get_messages();

  return $form['parameters'];
}

/**
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function apihub_ui_resources_form_js_test_handler_settings($form, $form_state) {
  drupal_get_messages();

  return $form['test']['handler_settings'];
}

/**
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function apihub_ui_resources_form_js_test_output($form, $form_state) {
  global $user;

  $resource = $form_state['item'];
  $values   = $form_state['values']['test'];

  // Store handler settings for this resource's api and current user.
  $handler_cid = "apihub:{$resource->api}:handler:{$user->uid}";
  cache_set($handler_cid, array(
    'handler'  => $values['handler'],
    'settings' => $values['handler_settings']
  ));

  // Store input values for this resource and current user.
  $input_cid = "apihub:{$resource->api}:input:{$resource->name}:{$user->uid}";
  cache_set($input_cid, $values['input']);

  // Process request.
  $params = array_filter($form_state['values']['test']['input']);
  $params = is_null($params) ? array() : $params;

  $request = new apihub_request($resource->api, $form_state['values']['test']['handler'], $form_state['values']['test']['handler_settings']);
  $result  = $request->execute($resource->method, $resource->path, $params);

  if (module_exists('devel')) {
    $form['test']['output']['#markup'] = kprint_r($result, TRUE);
  }
  else {
    $form['test']['output']['#markup'] = '<pre>' . print_r($result, TRUE) . '</pre>';
  }

  drupal_get_messages();

  return $form['test']['output'];
}

/**
 * @return array
 */
function apihub_ui_resources_handlers() {
  $handlers = apihub_handlers();

  $options = array();
  foreach ($handlers as $id => $handler) {
    $options[$id] = $handler['name'];
  }

  return $options;
}

/**
 * @param null $attribute
 *
 * @return bool
 */
function apihub_resources_ui_api($attribute = NULL) {
  $api = ctools_export_crud_load('apihub_apis', arg(3));

  if (!is_null($attribute)) {
    return isset($api->{$attribute}) ? $api->{$attribute} : FALSE;
  }

  return $api;
}
