<?php
/**
 * @file includes/go.hooks.inc
 */

/**
 * Implements hook_init().
 */
function go_init() {
  go_google_analytics_code_insert();
  go_node_to_frontpage();
}

/**
 * Implements of hook_element_info_alter().
 */
function go_element_info_alter(&$type) {
  global $conf;

  if (!empty($conf['go_text_formats'])) {
    // Our process callback must run immediately after filter_process_format().
    $filter_process_format_location = array_search('filter_process_format', $type['text_format']['#process']);
    $replacement = array('filter_process_format', 'go_filter_process_format');
    array_splice($type['text_format']['#process'], $filter_process_format_location, 1, $replacement);
  }
}

/**
 * Implements hook_go_api()
 */
function go_go_api() {
  return array('api' => '1.0');
}

/**
 * Implements hook_preprocess_page().
 * Simple 403/404 handlers
 */
function go_preprocess_page() {
  unset($_GET['destination']);
  go_preprocess_page__handle_40x();
}