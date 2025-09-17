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
    '#description' => t('Configure where email logs should be stored.'),
  );
  
  $form['logging_options']['xtd_mail_logger_database'] = array(
    '#type' => 'checkbox',
    '#title' => t('Log to database'),
    '#description' => t('Store email logs in the database for easy searching and filtering.'),
    '#default_value' => variable_get('xtd_mail_logger_database', TRUE),
  );
  
  $form['logging_options']['xtd_mail_logger_file'] = array(
    '#type' => 'checkbox',
    '#title' => t('Log to file'),
    '#description' => t('Store email logs in a file: sites/default/files/xtd_email_logger.log'),
    '#default_value' => variable_get('xtd_mail_logger_file', TRUE),
  );
  
  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Log Cleanup'),
    '#description' => t('Logs older than 7 days are automatically removed during cron runs.'),
  );
  
  // Show log statistics
  $db_count = db_query('SELECT COUNT(*) FROM {xtd_mail_logger}')->fetchField();
  $file_lines = 0;
  $log_file = 'sites/default/files/xtd_email_logger.log';
  if (file_exists($log_file)) {
    $file_lines = count(file($log_file));
  }
  
  $form['cleanup']['stats'] = array(
    '#type' => 'item',
    '#title' => t('Current log statistics'),
    '#markup' => t('Database entries: @db_count<br>File log lines: @file_lines', 
      array('@db_count' => number_format($db_count), '@file_lines' => number_format($file_lines))),
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
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  $form['integration']['info'] = array(
    '#type' => 'item',
    '#markup' => t('This module logs both successful and failed emails sent through drupal_mail(). 
      Logs include timestamp, status (PASS/FAIL), failure reason (if applicable), subject, from address, and to address.
      <br><br>View logs at: <a href="@logs_url">Administration » Reports » XTD Mail Logs</a>', 
      array('@logs_url' => url('admin/reports/xtd-mail-logs'))),
  );
  
  return system_settings_form($form);
}

/**
 * Form validation handler for xtd_mail_logger_admin_form().
 */
function xtd_mail_logger_admin_form_validate($form, &$form_state) {
  // Ensure at least one logging method is enabled
  if (!$form_state['values']['xtd_mail_logger_database'] && !$form_state['values']['xtd_mail_logger_file']) {
    form_set_error('logging_options', t('At least one logging method must be enabled.'));
  }
  
  // Check if file logging directory is writable
  if ($form_state['values']['xtd_mail_logger_file']) {
    $log_dir = 'sites/default/files';
    if (!is_writable($log_dir)) {
      form_set_error('xtd_mail_logger_file', t('The directory %dir is not writable. File logging cannot be enabled.', 
        array('%dir' => $log_dir)));
    }
  }
}

/**
 * Submit handler for clearing all logs.
 */
function xtd_mail_logger_clear_logs_submit($form, &$form_state) {
  xtd_mail_logger_clear_all_logs();
  $form_state['redirect'] = 'admin/config/development/xtd-mail-logger';
}
