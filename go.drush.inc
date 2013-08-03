<?php

/**
 * @file
 *   Go Drush commands
 */

/**
 * Implements hook_drush_command()
 * @return array
 */
function go_drush_command() {
  $items['go-download'] = array(
    'description' => 'Command to download libraries for you Drupal',
    'arguments' => array(
      'library' => 'Name of library to be downloaded.',
    ),
    'options' => array(
      'list' => 'List all supported libraries',
      'destination' => 'Directory you would like to download the library to.',
      'working-copy' => 'Preserves VCS directories, like .git, for projects downloaded using such methods.',
    ),
    'examples' => array(
      'drush godl --list' => 'List supported libraries.',
      'drush godl jquery.cycle' => 'Download the jquery.cycle library.',
    ),
    'aliases' => array('godl'),
    // No bootstrap is needed
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );

  return $items;
}

/**
 * Callback function to download the libraries for your Drupal.
 *
 * @param  string $library
 * @return boolean
 *
 * @see drush_invoke()
 * @see drush.api.php
 */
function drush_go_download($library = '') {
  // #####################
  // List the libraries
  // #####################
  if (drush_get_option('list', FALSE)) {
    $rows[] = array('Library', 'Name');
    $rows[] = array('---', '---');
    foreach (go_drush_get_supported_libraries() as $name => $details) {
      $rows[] = array($name, $details['name']);
    }
    $output = drush_format_table($rows, TRUE);
    return drush_print($output);
  }

  // #####################
  // Get params
  // #####################
  $destination = drush_get_option('destination', 'sites/all/libraries');

  // #####################
  // Download the library
  // #####################
  $library = go_drush_get_supported_library($library, $version = 'default');
  $class = DrushMakeProject::getInstance('GoLibrary', $library);
  if (!empty($destination)) {
    $class->contrib_destination = $destination;
  }
  $class->make();
}

/**
 * Override some default behaviors of parent class.
 */
class DrushMakeProject_GoLibrary extends DrushMakeProject_Library {
  /**
   * Override default value of parent.
   */
  protected function generatePath() {
    return parent::generatePath($base = FALSE);
  }
}

/**
 * Define supported libraries.
 *
 * @return array
 */
function go_drush_get_supported_libraries() {
  return array(
    'ckeditor' => array(
      'name' => 'CKEditor',
      'versions' => array(
        'default' => array('url' => 'http://download.cksource.com/CKEditor/CKEditor/CKEditor%203.6.6.1/ckeditor_3.6.6.1.zip'),
      ),
    ),
    'jquery.cycle' => array(
      'name' => 'jQuery cycle',
      'versions' => array(
        'default' => array('url' => 'http://malsup.github.io/min/jquery.cycle.all.min.js'),
        'lite' => array('url' => 'http://malsup.github.io/jquery.cycle.lite.js')
      ),
    ),
  );
}

/**
 * Build info for specific library.
 *
 * @param  string $name
 * @param  string $version
 * @return array
 */
function go_drush_get_supported_library($name, $version = 'default') {
  $libraries = go_drush_get_supported_libraries();

  if (!empty($libraries[$name])) {
    $library = $libraries[$name];

    if (!empty($library['versions'][$version])) {
      $version = $library['versions'][$version];

      if (defined('DRUPAL_ROOT')) {
        $subdir = !empty($library['subdir']) ? $library['subdir'] : '';
        $type = !empty($version['type']) ? $version['type'] : 'get';

        return array(
          'type' => 'library',
          'destination' => 'libraries',
          'name' => $name,
          'build_path' => DRUPAL_ROOT,
          'make_directory' => DRUPAL_ROOT,
          'contrib_destination' => 'sites/all',
          'subdir' => $subdir,
          'directory_name' => $name,
          'download' => array('type' => $type, 'url' => $version['url'])
        );
      }

      throw new Exception('DRUPAL_ROOT is not defined');
    }

    throw new Exception(sprintf('Version %s for %s is not supported', $version, $name));
  }

  throw new Exception('Library is not supported.');
}