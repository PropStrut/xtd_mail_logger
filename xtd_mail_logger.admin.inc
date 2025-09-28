<?php

/**
 * @file
 * Administration forms for XTD Mail Logger module.
 */

/**
 * Configuration form for XTD Mail Logger.
 */
function xtd_mail_logger_admin_form($form, &$form_state) {
  $form['logging_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Logging Options'),
    '#description' => t('Configure email logging settings.'),
  );
  
  $form['logging_options']['xtd_mail_logger_file'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable email logging'),
    '#description' => t('Store email logs in a file: sites/default/files/xtd_email_logger.log'),
    '#default_value' => variable_get('xtd_mail_logger_file', TRUE),
  );
  
  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Log Cleanup'),
  );
  
  $form['cleanup']['xtd_mail_logger_retention_days'] = array(
    '#type' => 'textfield',
    '#title' => t('Log retention period (days)'),
    '#description' => t('Logs older than this number of days will be automatically removed during cron runs. Set to 0 to disable automatic cleanup.'),
    '#default_value' => variable_get('xtd_mail_logger_retention_days', 3),
    '#size' => 10,
    '#maxlength' => 3,
    '#required' => TRUE,
  );
  
  $retention_days = variable_get('xtd_mail_logger_retention_days', 3);
  $next_cron = variable_get('cron_last', 0) + 3600; // Estimate next cron run
  
  $form['cleanup']['info'] = array(
    '#type' => 'item',
    '#title' => t('Cleanup information'),
    '#markup' => t('Current retention period: @days days<br>Next cleanup will run during the next cron execution.<br>Last cron run: @last_cron', 
      array(
        '@days' => $retention_days,
        '@last_cron' => variable_get('cron_last', 0) ? format_date(variable_get('cron_last', 0), 'short') : t('Never'),
      )),
  );
  
  // Show log statistics
  $file_lines = 0;
  $log_file = 'sites/default/files/xtd_email_logger.log';
  if (file_exists($log_file)) {
    $file_lines = count(file($log_file));
  }
  
  $form['cleanup']['stats'] = array(
    '#type' => 'item',
    '#title' => t('Current log statistics'),
    '#markup' => t('File log lines: @file_lines<br>File size: @file_size', 
      array(
        '@file_lines' => number_format($file_lines), 
        '@file_size' => file_exists($log_file) ? format_size(filesize($log_file)) : '0 bytes'
      )),
  );
  
  $form['cleanup']['clear_logs'] = array(
    '#type' => 'submit',
    '#value' => t('Clear all logs'),
    '#submit' => array('xtd_mail_logger_clear_logs_submit'),
    '#attributes' => array(
      'onclick' => 'return confirm("Are you sure you want to clear all logs? This cannot be undone.");',
    ),
  );
  
  $form['integration'] = array(
    '#type' => 'fieldset',
    '#title' => t('Integration Information'),
    '#description' => t('XTD Mail Logger automatically integrates with Drupal\'s mail system to log all outgoing emails.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  
  $form['integration']['info'] = array(
    '#type' => 'item',
    '#markup' => t('This module logs both successful and failed emails sent through drupal_mail(). 
      Logs include timestamp, status (PASS/FAIL), module/key (email source), failure reason (if applicable), subject, from address, and to address.
      <br><br>View logs at: <a href="@logs_url">Administration » Reports » XTD Mail Logs</a>', 
      array('@logs_url' => url('admin/reports/xtd-mail-logs'))),
  );
  
  return system_settings_form($form);
}

/**
 * Form validation handler for xtd_mail_logger_admin_form().
 */
function xtd_mail_logger_admin_form_validate($form, &$form_state) {
  // Check if file logging directory is writable
  if ($form_state['values']['xtd_mail_logger_file']) {
    $log_dir = 'sites/default/files';
    if (!is_writable($log_dir)) {
      form_set_error('xtd_mail_logger_file', t('The directory %dir is not writable. File logging cannot be enabled.', 
        array('%dir' => $log_dir)));
    }
  }
  
  // Validate retention days
  $retention_days = $form_state['values']['xtd_mail_logger_retention_days'];
  if (!is_numeric($retention_days) || $retention_days < 0) {
    form_set_error('xtd_mail_logger_retention_days', t('Retention period must be a number greater than or equal to 0.'));
  }
  if ($retention_days > 365) {
    form_set_error('xtd_mail_logger_retention_days', t('Retention period cannot be more than 365 days.'));
  }
}

/**
 * Submit handler for clearing all logs.
 */
function xtd_mail_logger_clear_logs_submit($form, &$form_state) {
  xtd_mail_logger_clear_all_logs();
  $form_state['redirect'] = 'admin/config/development/xtd-mail-logger';
}
