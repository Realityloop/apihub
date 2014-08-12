<?php

/**
 * Class apihub_resources_ui
 */
class apihub_resources_ui extends apihub_ui {

  /**
   * Builds the operation links for a specific exportable item.
   */
  function build_operations($item) {
    $allowed_operations = parent::build_operations($item);

    foreach ($allowed_operations as &$operation) {
      $operation['href'] = str_replace('%', arg(4), $operation['href']);
    }

    return $allowed_operations;
  }

  /**
   * Deletes exportable items from the database.
   */
  function delete_form_submit(&$form_state) {
    $item = $form_state['item'];

    // Clear CTools object cache.
    ctools_include('object-cache');
    ctools_object_cache_clear('apihub_ui_resources', $item->name);

    parent::delete_form_submit($form_state);
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

    // Adjust plugin menu prefix.
    $form_state['plugin']['menu']['menu prefix'] = str_replace('%', $item->api, $form_state['plugin']['menu']['menu prefix']);

    // Include the CTools tools that we need.
    ctools_include('ajax');
    ctools_include('object-cache');
    ctools_include('modal');

    // Set the object into CTools object cache.
    $cache = ctools_object_cache_get('apihub_ui_resources', $item->name);
    if ($cache) {
      $item = $cache;
    }
    else {
      ctools_object_cache_set('apihub_ui_resources', $item->name, $item);
    }

    // Ensure form is cacheable for custom ajax.
    $form_state['cache'] = TRUE;

    // Add CTools' javascript to the page.
    ctools_modal_add_js();

    if (empty($form_state['values'])) {
      $api = ctools_export_crud_load('apihub_apis', arg(4));
    }
    else {
      $api = ctools_export_crud_load('apihub_apis', $form_state['values']['api']);
    }

    if ($form_state['op'] !== 'edit') {
      unset($form['info'][$export_key]);
    }

    // Hide machine name.
    $form['info']['name']['#type'] = 'value';

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
    $form['parameters'] = $this->edit_form_fields($item, 'parameters', t('Parameters'));

    // Results.
    $form['results'] = $this->edit_form_fields($item, 'results', t('Results'));

    // Advanced settings.
    $form['settings'] = array(
      '#type'        => 'container',
      '#title'       => t('Advanced settings'),
      '#tree'        => TRUE,
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
    );

    if (!empty($item->settings)) {
      foreach ($item->settings as $module => $settings) {
        if (!module_exists($module)) {
          $form['settings'][$module] = array(
            '#type'  => 'value',
            '#value' => $settings,
          );
        }
      }
    }

    $settings = FALSE;
    foreach (module_implements('apihub_resources_settings') as $module) {
      $settings                  = TRUE;
      $form['settings'][$module] = module_invoke($module, 'apihub_resources_settings', $item->settings, $item);
    }

    if ($settings) {
      $form['settings']['#type'] = 'fieldset';
    }
  }

  /**
   * @param $item
   * @param $type
   * @param $title
   *
   * @return array
   */
  function edit_form_fields($item, $type, $title) {
    // @TODO - Toggle edit mode/summary mode.
    $element = array(
      '#type'   => 'fieldset',
      '#title'  => $title,
      '#tree'   => TRUE,
      '#theme'  => 'apihub_ui_resources_edit_form_fields',
      '#prefix' => "<div id='{$type}-wrapper''>",
      '#suffix' => '</div>',
    );

    // Ensure fields are in the form of an array.
    if (!isset($item->{$type}) || is_null($item->{$type})) {
      $item->{$type} = array();
    }

    foreach ($item->{$type} as $id => $field) {
      foreach ($field as $name => $value) {
        $element[$id][$name] = array(
          '#type'  => 'value',
          '#value' => $value,
        );
      }

      $element[$id]['id'] = array(
        '#type'          => 'hidden',
        '#default_value' => $field['id'],
      );

      $element[$id]['pid'] = array(
        '#type'          => 'hidden',
        '#default_value' => $field['pid'],
      );

      $element[$id]['weight'] = array(
        '#type'          => 'weight',
        '#title'         => t('Weight'),
        '#title_display' => 'invisible',
        '#default_value' => $field['weight'],
        '#delta'         => 10,
      );

      $element[$id]['_edit_url'] = array(
        '#type'       => 'hidden',
        '#attributes' => array('class' => array("{$type}-{$id}-edit-button-url")),
        '#value'      => url("admin/structure/apihub/list/{$item->api}/resources/list/{$item->name}/field/{$type}/{$id}"),
      );

      $element[$id]['_edit'] = array(
        '#type'       => 'button',
        '#value'      => t('Edit'),
        '#attributes' => array('class' => array('ctools-use-modal')),
        '#id'         => "{$type}-{$id}-edit-button",
      );
    }

    $element['_url'] = array(
      '#type'       => 'hidden',
      '#attributes' => array('class' => array("{$type}-add-button-url")),
      '#value'      => url("admin/structure/apihub/list/{$item->api}/resources/list/{$item->name}/field/{$type}"),
    );

    $element['_add'] = array(
      '#type'       => 'button',
      '#value'      => t('Add another @type', array('@type' => drupal_substr($type, 0, -1))),
      '#attributes' => array('class' => array('ctools-use-modal')),
      '#id'         => "{$type}-add-button",
    );

    return $element;
  }

  /**
   * Validate callback for the edit form.
   */
  function edit_form_validate(&$form, &$form_state) {
    parent::edit_form_validate($form, $form_state);

    // Sanitize data.
    foreach (array('parameters', 'results') as $type) {
      // Sort fields by $_POST order.
      $order                       = array_flip(array_keys($form_state['input'][$type]));
      $form_state['values'][$type] = array_merge($order, $form_state['values'][$type]);
      $form_state['item']->$type   = array_merge($order, $form_state['item']->$type);

      // Remove fields begining with an underscore ('_').
      foreach (array_keys($form_state['values'][$type]) as $field_id) {
        if (strpos($field_id, '_') === 0) {
          unset($form_state['values'][$type][$field_id]);
          continue;
        }
        foreach (array_keys($form_state['values'][$type][$field_id]) as $param) {
          if (strpos($param, '_') === 0) {
            unset($form_state['values'][$type][$field_id][$param]);
          }
        }
      }
    }

    $export_key = $this->plugin['export']['key'];
    // Build the export key.
    if ($form_state['op'] != 'edit') {
      $form_state['values'][$export_key] = md5("{$form_state['values']['api']}::{$form_state['values']['method']}::{$form_state['values']['path']}");
    }

    // Clear CTools object cache.
    ctools_include('object-cache');
    ctools_object_cache_clear('apihub_ui_resources', $form_state['values'][$export_key]);
  }

  /**
   * @param $js
   * @param $input
   * @param $item
   * @param $type
   *
   * @return array|mixed
   */
  function field_page($js, $input, $item, $type, $id) {
    // Fall back if $js is not set.
    if (!$js) {
      return drupal_get_form('apihub_ui_resources_field_form');
    }

    ctools_include('ajax');
    ctools_include('modal');
    ctools_include('object-cache');

    $cache = ctools_object_cache_get('apihub_ui_resources', $item->name);
    if ($cache) {
      $item = $cache;
    }

    $title = t('Add another @type', array('@type' => drupal_substr($type, 0, -1)));
    if (!empty($id)) {
      $title = t('Edit @type "@id"', array(
        '@type' => drupal_substr($type, 0, -1),
        '@id'   => $id,
      ));
    }

    $form_state = array(
      'title'         => $title,
      'ajax'          => TRUE,
      'form_build_id' => $input['form_build_id'],
    );
    $output     = ctools_modal_form_wrapper('apihub_ui_resources_field_form', $form_state);
    if (!empty($form_state['executed'])) {
      $id = substr($type, 0, 1) . count($item->{$type});

      // Remove old entry.
      if (isset($item->{$type}[$form_state['values']['id']])) {
        $id = $form_state['values']['id'];
        unset($item->{$type}[$id]);
      }

      // Add new entry.
      if ($form_state['clicked_button']['#value'] == t('Submit')) {
        $item->{$type}[$id] = array(
          'id'          => $id,
          'name'        => $form_state['values']['name'],
          'type'        => $form_state['values']['type'],
          'description' => $form_state['values']['description'],
          'weight'      => $form_state['values']['weight'],
        );
        if ($type == 'parameters') {
          $item->{$type}[$id]['options']  = $form_state['values']['options'];
          $item->{$type}[$id]['required'] = $form_state['values']['required'];
        }
        elseif ($type == 'results') {
          $item->{$type}[$id]['offline'] = $form_state['values']['offline'];
        }
      }

      // Modify and cache the resource item.
      ctools_object_cache_set('apihub_ui_resources', $item->name, $item);

      // Regenerate parent form from cache.
      $_POST['form_build_id'] = $input['parent_build_id'];
      list($form, $form_state, $form_id, $form_build_id, $commands) = ajax_get_form();

      $form_state['rebuild_info']['copy']['#build_id'] = $input['parent_build_id'];
      $old_form                                        = array('#build_id' => $input['parent_build_id']);
      drupal_process_form($form['#form_id'], $form, $form_state);
      $form = drupal_rebuild_form($form['#form_id'], $form_state, $old_form);

      // Build AJAX commands.
      $output   = $commands;
      $output[] = ctools_modal_command_dismiss();
      $output[] = array(
        'command'  => 'insert',
        'method'   => NULL,
        'selector' => "#{$type}-wrapper",
        'data'     => render($form[$type]),
        'settings' => array(),
      );
    }
    print ajax_render($output);
    exit;
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
    if ($item->api != arg(4)) {
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

  /**
   * @param      $js
   * @param      $input
   * @param      $item
   * @param null $step
   *
   * @return array|mixed
   */
  function test_page($js, $input, $item, $step = NULL) {
    drupal_set_title($this->get_page_title('test', $item));

    $form_state = array(
      'plugin'        => $this->plugin,
      'object'        => &$this,
      'ajax'          => $js,
      'item'          => $item,
      'op'            => 'test',
      'form type'     => 'test',
      'rerender'      => TRUE,
      'no_redirect'   => TRUE,
      'step'          => $step,
      // Store these in case additional args are needed.
      'function args' => func_get_args(),
    );

    $output = drupal_build_form('apihub_ui_resources_form_test', $form_state);
    //    if (!empty($form_state['executed'])) {
    //      $this->delete_form_submit($form_state);
    //      $this->redirect($form_state['op'], $item);
    //    }

    return $output;
  }
}

/**
 * Test form callback.
 *
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function apihub_ui_resources_form_test($form, $form_state) {
  global $user;
  $item = $form_state['item'];

  // Handler.
  $handler_cid   = "apihub:{$item->api}:handler:{$user->uid}";
  $handler_cache = cache_get($handler_cid, 'cache_apihub');

  $form['handler'] = array(
    '#type'    => 'select',
    '#title'   => t('Handler'),
    '#options' => apihub_ui_resources_handlers(),
    '#ajax'    => array(
      'callback' => 'apihub_ui_resources_form_js_test_handler_settings',
      'wrapper'  => 'test-handler-settings-output',
    ),
  );

  $form['handler_settings'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Handler settings'),
    '#tree'        => TRUE,
    '#collapsible' => TRUE,
    '#prefix'      => '<div id="test-handler-settings-output">',
    '#suffix'      => '</div>',
  );

  // Determine which handler is currently in use.
  $handler = isset($form_state['values']['handler']) ? $form_state['values']['handler'] : key($form['handler']['#options']);
  if (isset($handler_cache->data['handler']) && in_array($handler_cache->data['handler'], array_keys($form['handler']['#options']))) {
    $handler                           = $handler_cache->data['handler'];
    $form['handler']['#default_value'] = $form['handler'];
  }

  $handler = apihub_handlers_load($handler);
  if ($handler && isset($handler['settings']) && !empty($handler['settings'])) {
    foreach ($handler['settings'] as $id => $setting) {
      $form['handler_settings'][$id] = _apihub_field_to_fapi($setting);
      if (isset($handler_cache->data['settings'][$id])) {
        $form['handler_settings'][$id]['#default_value'] = $handler_cache->data['settings'][$id];
      }
    }
  }

  // Input.
  $input_cid   = "apihub:{$item->api}:input:{$item->name}:{$user->uid}";
  $input_cache = cache_get($input_cid, 'cache_apihub');

  $form['input'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Input'),
    '#tree'        => TRUE,
    '#collapsible' => TRUE,
  );

  foreach ($item->parameters as $parameter) {
    if (empty($parameter)) {
      continue;
    }

    $form['input'][$parameter['id']] = _apihub_field_to_fapi($parameter);
    if (isset($input_cache->data[$parameter['id']])) {
      $form['input'][$parameter['id']]['#default_value'] = $input_cache->data[$parameter['id']];
    }
  }
  $inputs = element_children($form['input']);
  if (empty($inputs)) {
    unset($form['input']);
  }

  // Output.
  $form['output'] = array(
    '#type'   => 'item',
    '#title'  => t('Output'),
    '#markup' => t('No data'),
    '#prefix' => '<div id="test-output-wrapper">',
    '#suffix' => '</div>',
  );

  $form['submit'] = array(
    '#type'  => 'button',
    '#value' => $item->method,
    '#ajax'  => array(
      'callback' => 'apihub_ui_resources_form_js_test_output',
      'wrapper'  => 'test-output-wrapper',
    ),
  );

  return $form;
}

/**
 * Test form handler settings AJAX callback.
 *
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function apihub_ui_resources_form_js_test_handler_settings($form, $form_state) {
  drupal_get_messages();

  return $form['handler_settings'];
}

/**
 * Test form output AJAX callback.
 *
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function apihub_ui_resources_form_js_test_output($form, $form_state) {
  global $user;

  $resource = $form_state['item'];
  $values   = $form_state['values'];

  // Store handler settings for this resource's api and current user.
  $handler_cid = "apihub:{$resource->api}:handler:{$user->uid}";
  cache_set($handler_cid, array(
    'handler'  => $values['handler'],
    'settings' => $values['handler_settings']
  ), 'cache_apihub');

  // Store input values for this resource and current user.
  $input_cid = "apihub:{$resource->api}:input:{$resource->name}:{$user->uid}";
  cache_set($input_cid, $values['input'], 'cache_apihub');

  // Clear messages.
  drupal_get_messages();

  // Process request.
  $request = new apihub_request($resource->api, $values['handler'], $values['handler_settings']);
  $result  = $request->execute($resource->method, $resource->path, $form_state['values']['input']);

  if (module_exists('devel')) {
    $form['output']['#markup'] = kprint_r($result, TRUE);
  }
  else {
    $form['output']['#markup'] = '<pre>' . print_r($result, TRUE) . '</pre>';
  }

  return $form['output'];
}

/**
 * API Hub handlers to options array.
 *
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
 * Get resource API or API attribute.
 *
 * @param null $attribute
 *
 * @return bool
 */
function apihub_resources_ui_api($attribute = NULL) {
  $api = ctools_export_crud_load('apihub_apis', arg(4));

  if (!is_null($attribute)) {
    return isset($api->{$attribute}) ? $api->{$attribute} : FALSE;
  }

  return $api;
}
