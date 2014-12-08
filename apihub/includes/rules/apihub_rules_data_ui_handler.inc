<?php
/**
 * @file
 * UI for API Hub handler.
 */

/**
 * Class APIHubRulesDataUIHandler
 */
class APIHubRulesDataUIHandler extends RulesDataUI implements RulesDataDirectInputFormInterface {
  public static function getDefaultMode() {
    return 'input';
  }

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form[$name] = array(
      '#type' => 'container',
      '#tree' => TRUE,
    );

    $defaults = isset($element->settings['handler']) ? $element->settings['handler'] : NULL;
    $form[$name] += apihub_handlers_form($defaults);

    return $form;
  }

  public static function render($value) {
    return array(
      // @TODO - Add more informative description of handler and settings used.
      'content'     => array('#markup' => check_plain($value['handler'])),
      '#attributes' => array('class' => array('rules-parameter-text')),
    );
  }
}
