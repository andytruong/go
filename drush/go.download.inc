<?php

/**
 * @file ./drush/go.download.inc
 *
 * @author Andy Truong
 */

function go_drush_download_command_info() {
  $items = array();

  if (version_compare(DRUSH_MAJOR_VERSION, 5) >= 0) {
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
  }

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
function drush_go_download($library = '', $version = 'default') {
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
  $library = go_drush_get_supported_library($library, $version);
  $class = DrushMakeProject::getInstance('GoLibrary', $library);
  if (!empty($destination)) {
    $class->contrib_destination = $destination;
  }

  $class->make();
}

/**
 * Override some default behaviors of parent class.
 * At current, happening error when enable go module
 * Error: Class 'DrushMakeProject_Library' not found in ...
 * Require class 'DrushMakeProject_Library' at sites/all/modules/drush/commands/make/make.project.inc, line 453
 *
 * @see http://api.drupalize.me/api/drupal/class/DrushMakeProject_Library/7
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
    'colorbox' => array(
      'name' => 'Colorbox',
      'versions' => array(
        'default' => array('url' => 'https://github.com/jackmoore/colorbox/archive/1.x.zip'),
        '2.x' => array('url' => 'https://github.com/jackmoore/colorbox/archive/2.x.zip'),
      ),
    ),
    'mailchimp' => array(
      'name' => 'Mailchimp PHP library',
      'versions' => array(
        'default' => array('url' => 'http://apidocs.mailchimp.com/api/downloads/mailchimp-api-class.zip')
      ),
    ),
    'jquery.cycle.2' => array(
      'name' => 'jQuery cycle',
      'versions' => array(
        'default' => array('url' => 'http://malsup.github.io/min/jquery.cycle2.min.js'),
      ),
    ),
    'jquery.cycle' => array(
      'name' => 'jQuery cycle',
      'versions' => array(
        'default' => array('url' => 'http://malsup.github.io/min/jquery.cycle.all.min.js'),
        'lite' => array('url' => 'http://malsup.github.io/jquery.cycle.lite.js')
      ),
    ),
    'maxmind.geolite.country' => array(
      'name' => 'Maxmind GeoLite Country',
      'versions' => array(
        'default' => 'http://geolite.maxmind.com/download/geoip/database/GeoLiteCountry/GeoIP.dat.gz',
        'ipv6' => 'http://geolite.maxmind.com/download/geoip/database/GeoIPv6.dat.gz',
      ),
    ),
    'maxmind.geolite.city' => array(
      'name' => 'Maxmind GeoLite City',
      'versions' => array(
        'default' => 'http://geolite.maxmind.com/download/geoip/database/GeoLiteCity.dat.gz',
        'ipv6' => 'http://geolite.maxmind.com/download/geoip/database/GeoLiteCityv6-beta/GeoLiteCityv6.dat.gz',
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
