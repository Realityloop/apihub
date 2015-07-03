<?php
/**
 * @file
 * OAuth Connector module integration.
 */

/**
 * Implements hook_apihub_handler_info() on behalf of oauthconnector.module.
 */
function oauthconnector_apihub_handler_info() {
  global $user;

  return array(
    'oauthconnector' => array(
      'name'      => t('OAuth Connector'),
      'class'     => 'oauthconnector_apihub_handler',
      'settings'  => array(
        'provider' => array(
          'type'             => 'text',
          'name'             => t('Provider'),
          'options callback' => 'oauthconnector_apihub_handler_settings_provider_options',
          'required'         => TRUE,
        ),
        'account'  => array(
          'type'     => 'user',
          'name'     => t('User'),
          'required' => TRUE,
        ),
      ),
//      'callbacks' => array(
//        'methods ui handler settings' => '',
//      ),
    ),
  );
}

/**
 * Options callback for Provider setting.
 *
 * @return array
 */
function oauthconnector_apihub_handler_settings_provider_options() {
  $options = array();

  $providers = oauthconnector_provider_load_all();
  foreach ($providers as $provider) {
    $options[$provider->name] = $provider->title;
  }

  return $options;
}

/**
 *
 */
//function oauthconnector_apihub_handler_settings() {
//  global $user;
//
//  $form['account'] = array(
//    '#type'              => 'textfield',
//    '#title'             => t('User'),
//    '#size'              => 32,
//    '#maxlength'         => 60,
//    '#autocomplete_path' => 'user/autocomplete',
//    '#required'          => TRUE,
//    '#default_value'     => $user->name,
//  );
//
//  $providers = oauthconnector_provider_load_all();
//  $options   = array();
//  foreach ($providers as $provider) {
//    $options[$provider->name] = $provider->title;
//  }
//  $form['provider'] = array(
//    '#type'    => 'select',
//    '#title'   => 'Provider',
//    '#options' => $options,
//  );
//
//  return $form;
//}

/**
 * Class oauthconnector_apihub_handler
 */
class oauthconnector_apihub_handler extends apihub_handler {
  protected $account = NULL;
  protected $provider = NULL;

  /**
   * @param array $args
   */
  function __construct($args = array()) {
    if (isset($args['account'])) {
      $this->account = $args['account'];
    }

    if (isset($args['provider'])) {
      $this->provider = $args['provider'];
    }
  }

  /**
   * @param       $method
   * @param       $url
   * @param array $params
   *
   * @return array|null
   */
  function execute($method, $url, $params = array(), $method) {
    return oauthconnector_endpoint_call_for_user($url, $params, $this->provider, $this->account, $method);
  }
}
