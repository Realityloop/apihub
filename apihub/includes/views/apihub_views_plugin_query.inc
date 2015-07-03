<?php

/**
 * Class apihub_views_plugin_query
 */
class apihub_views_plugin_query extends views_plugin_query {

  /**
   * Add a field to the query table, possibly with an alias.
   *
   * @param $table
   * @param $field
   * @param $alias
   * @param $params
   *
   * @return $name
   *   The name that this field can be referred to as. Usually this is the alias.
   */
  function add_field($table, $field, $alias = '', $params = array()) {
    return $field;
  }

  /**
   * Add a complex WHERE clause to the query.
   *
   * The caller is reponsible for ensuring that all fields are fully qualified
   * (TABLE.FIELD) and that the table already exists in the query.
   * Internally the dbtng method "where" is used.
   *
   * @param $group
   *   The WHERE group to add these to; groups are used to create AND/OR
   *   sections. Groups cannot be nested. Use 0 as the default group.
   *   If the group does not yet exist it will be created as an AND group.
   * @param $snippet
   *   The snippet to check. This can be either a column or
   *   a complex expression like "UPPER(table.field) = 'value'"
   * @param $args
   *   An associative array of arguments.
   *
   * @see QueryConditionInterface::where()
   */
  function add_where_expression($group, $snippet, $args = array()) {
    // Ensure all variants of 0 are actually 0. Thus '', 0 and NULL are all
    // the default group.
    if (empty($group)) {
      $group = 0;
    }

    // Check for a group.
    if (!isset($this->where[$group])) {
      $this->set_where_group('AND', $group);
    }

    $this->where[$group]['conditions'][] = array(
      'field'    => $snippet,
      'value'    => $args,
      'operator' => 'formula',
    );
  }

  /**
   * Ensure a table exists in the queue; Currently does nothing..
   *
   * @param            $table
   *   The unaliased name of the table to ensure.
   * @param            $relationship
   *   The relationship to ensure the table links to. Each relationship will
   *   get a unique instance of the table being added. If not specified,
   *   will be the primary table.
   * @param views_join $join
   *   A views_join object (or derived object) to join the alias in.
   *
   * @return
   *   The alias used to refer to this specific table, or NULL if the table
   *   cannot be ensured.
   */
  function ensure_table($table, $relationship = NULL, $join = NULL) {
  }

  /**
   * Executes the query and fills the associated view object with according
   * values.
   *
   * Values to set: $view->result, $view->total_rows, $view->execute_time,
   * $view->pager['current_page'].
   *
   * $view->result should contain an array of objects. The array must use a
   * numeric index starting at 0.
   *
   * @param view $view
   *   The view which is executed.
   */
  function execute(&$view) {
    if (!isset($view->query->options['handler'])) {
      drupal_set_message(t('API Hub handler has not been configured. Set in !link.', array(
        '!link' => $view->display[$view->current_display]->handler->option_link(t('Query Settings'), 'query'),
      )));

      return;
    }

    // Load current method.
    ctools_include('export');
    $method = ctools_export_crud_load('apihub_methods', substr($view->base_table, 7));

    // Process the request.
    $request = new apihubRequest($method->api, $view->query->options['handler']['handler'], $view->query->options['handler']['settings']);
    $result  = $request->execute($method->http_method, $method->path, array());

    // Set root element.
    if (!empty($method->settings['apihub']['root'])) {
      $result = $result->{$method->settings['apihub']['root']};
    }

    foreach ($result as $key => $item) {
      // Filters.
      $view->result[] = $item;
    }
  }

  /**
   * Add settings for the ui.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $defaults        = isset($form_state['view']->query->options['handler']) ? $form_state['view']->query->options['handler'] : array();
    $form['handler'] = apihub_handlers_form($defaults);
  }


  /**
   * Generate a query and a countquery from all of the information supplied
   * to the object.
   *
   * @param $get_count
   *   Provide a countquery if this is true, otherwise provide a normal query.
   */
  function query($get_count = FALSE) {

  }

}
