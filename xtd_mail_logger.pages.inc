<?php

/**
 * @file
 * Page callbacks for XTD Mail Logger module - File logging only.
 */

/**
 * Main logs page callback.
 */
function xtd_mail_logger_logs_page() {
  // Go directly to file logs since we only have file logging now
  return xtd_mail_logger_file_logs_page();
}

/**
 * File logs page callback.
 */
function xtd_mail_logger_file_logs_page() {
  $build = array();
  
  // Check if file logging is enabled
  if (!variable_get('xtd_mail_logger_file', TRUE)) {
    $build['disabled'] = array(
      '#markup' => '<p>' . t('File logging is currently disabled. Enable it in the <a href="@config">configuration</a>.', 
        array('@config' => url('admin/config/development/xtd-mail-logger'))) . '</p>',
    );
    return $build;
  }
  
  // Add filter form
  $build['filter_form'] = drupal_get_form('xtd_mail_logger_filter_form');
  
  // Get filters from query parameters
  $filters = array();
  if (!empty($_GET['status'])) {
    $filters['status'] = $_GET['status'];
  }
  if (!empty($_GET['from_date'])) {
    $filters['from_date'] = $_GET['from_date'];
  }
  if (!empty($_GET['to_date'])) {
    $filters['to_date'] = $_GET['to_date'];
  }
  if (!empty($_GET['search'])) {
    $filters['search'] = $_GET['search'];
  }
  
  // Build the file logs display
  $build['logs_display'] = xtd_mail_logger_build_file_display($filters);
  
  return $build;
}

/**
 * Filter form for log pages.
 */
function xtd_mail_logger_filter_form($form, &$form_state) {
  $form['filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filter logs'),
    '#collapsible' => TRUE,
    '#collapsed' => empty($_GET['status']) && empty($_GET['from_date']) && empty($_GET['to_date']) && empty($_GET['search']),
    '#attributes' => array('class' => array('xtd-mail-logger-filters')),
  );
  
  $form['filters']['status'] = array(
    '#type' => 'select',
    '#title' => t('Status'),
    '#options' => array(
      '' => t('- Any -'),
      'PASS' => t('Pass'),
      'FAIL' => t('Fail'),
    ),
    '#default_value' => isset($_GET['status']) ? $_GET['status'] : '',
  );
  
  $form['filters']['from_date'] = array(
    '#type' => 'textfield',
    '#title' => t('From date'),
    '#description' => t('Format: YYYY-MM-DD'),
    '#size' => 12,
    '#maxlength' => 10,
    '#default_value' => isset($_GET['from_date']) ? $_GET['from_date'] : '',
    '#attributes' => array('placeholder' => date('Y-m-d', strtotime('-7 days'))),
  );
  
  $form['filters']['to_date'] = array(
    '#type' => 'textfield',
    '#title' => t('To date'),
    '#description' => t('Format: YYYY-MM-DD'),
    '#size' => 12,
    '#maxlength' => 10,
    '#default_value' => isset($_GET['to_date']) ? $_GET['to_date'] : '',
    '#attributes' => array('placeholder' => date('Y-m-d')),
  );
  
  $form['filters']['search'] = array(
    '#type' => 'textfield',
    '#title' => t('Search'),
    '#description' => t('Search in module/key, subject, from, to, or failure reason'),
    '#default_value' => isset($_GET['search']) ? $_GET['search'] : '',
    '#size' => 30,
  );
  
  $form['filters']['actions'] = array(
    '#type' => 'actions',
  );
  
  $form['filters']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
  );
  
  $form['filters']['actions']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#submit' => array('xtd_mail_logger_filter_form_reset'),
  );
  
  return $form;
}

/**
 * Validate handler for filter form.
 */
function xtd_mail_logger_filter_form_validate($form, &$form_state) {
  // Validate date formats
  if (!empty($form_state['values']['from_date'])) {
    if (!preg_match('/^\d{4}-\d{2}-\d{2}$/', $form_state['values']['from_date']) || 
        !strtotime($form_state['values']['from_date'])) {
      form_set_error('from_date', t('From date must be in YYYY-MM-DD format.'));
    }
  }
  
  if (!empty($form_state['values']['to_date'])) {
    if (!preg_match('/^\d{4}-\d{2}-\d{2}$/', $form_state['values']['to_date']) || 
        !strtotime($form_state['values']['to_date'])) {
      form_set_error('to_date', t('To date must be in YYYY-MM-DD format.'));
    }
  }
  
  // Validate date range
  if (!empty($form_state['values']['from_date']) && !empty($form_state['values']['to_date'])) {
    $from_timestamp = strtotime($form_state['values']['from_date']);
    $to_timestamp = strtotime($form_state['values']['to_date']);
    
    if ($from_timestamp > $to_timestamp) {
      form_set_error('to_date', t('To date must be after from date.'));
    }
  }
}

/**
 * Submit handler for filter form.
 */
function xtd_mail_logger_filter_form_submit($form, &$form_state) {
  $query = array();
  
  if (!empty($form_state['values']['status'])) {
    $query['status'] = $form_state['values']['status'];
  }
  if (!empty($form_state['values']['from_date'])) {
    $query['from_date'] = $form_state['values']['from_date'];
  }
  if (!empty($form_state['values']['to_date'])) {
    $query['to_date'] = $form_state['values']['to_date'];
  }
  if (!empty($form_state['values']['search'])) {
    $query['search'] = $form_state['values']['search'];
  }
  
  $form_state['redirect'] = array(
    current_path(),
    array('query' => $query),
  );
}

/**
 * Reset submit handler for filter form.
 */
function xtd_mail_logger_filter_form_reset($form, &$form_state) {
  $form_state['redirect'] = current_path();
}

/**
 * Build file logs display.
 */
function xtd_mail_logger_build_file_display($filters = array()) {
  $log_file = 'sites/default/files/xtd_email_logger.log';
  
  $build = array();
  
  if (!file_exists($log_file)) {
    $build['empty'] = array(
      '#markup' => '<p>' . t('No log file found. The log file will be created when the first email is sent.') . '</p>',
    );
    return $build;
  }
  
  $lines = file($log_file, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
  if (empty($lines)) {
    $build['empty'] = array(
      '#markup' => '<p>' . t('The log file is empty.') . '</p>',
    );
    return $build;
  }
  
  $parsed_logs = array();
  $total_lines = count($lines);
  
  // Parse log file lines
  foreach ($lines as $line_number => $line) {
    $line = trim($line);
    if (empty($line)) {
      continue;
    }
    
    // Parse the log format: [YYYY-MM-DD HH:MM:SS] STATUS | Module: module/key | Subject: ... | From: ... | To: ... | Reason: ...
    if (preg_match('/^\[([^\]]+)\]\s+(\w+)\s*\|\s*Module:\s*([^|]*?)\s*\|\s*Subject:\s*(.*?)\s*\|\s*From:\s*(.*?)\s*\|\s*To:\s*(.*?)\s*\|\s*Reason:\s*(.*)$/', $line, $matches)) {
      $timestamp = strtotime($matches[1]);
      
      // Skip if we can't parse the timestamp
      if ($timestamp === FALSE) {
        continue;
      }
      
      $log = array(
        'timestamp' => $timestamp,
        'status' => trim($matches[2]),
        'module_key' => trim($matches[3]),
        'subject' => trim($matches[4]),
        'from_email' => trim($matches[5]),
        'to_email' => trim($matches[6]),
        'failure_reason' => trim($matches[7]),
        'raw_line' => $line,
        'line_number' => $line_number + 1,
      );
      
      // Apply filters
      $include = TRUE;
      
      if (!empty($filters['status']) && $log['status'] != $filters['status']) {
        $include = FALSE;
      }
      
      if ($include && !empty($filters['from_date'])) {
        $from_timestamp = strtotime($filters['from_date'] . ' 00:00:00');
        if ($from_timestamp && $log['timestamp'] < $from_timestamp) {
          $include = FALSE;
        }
      }
      
      if ($include && !empty($filters['to_date'])) {
        $to_timestamp = strtotime($filters['to_date'] . ' 23:59:59');
        if ($to_timestamp && $log['timestamp'] > $to_timestamp) {
          $include = FALSE;
        }
      }
      
      if ($include && !empty($filters['search'])) {
        $search_text = strtolower($filters['search']);
        $log_text = strtolower(implode(' ', array(
          isset($log['module_key']) ? $log['module_key'] : '',
          $log['subject'], 
          $log['from_email'], 
          $log['to_email'], 
          $log['failure_reason']
        )));
        if (strpos($log_text, $search_text) === FALSE) {
          $include = FALSE;
        }
      }
      
      if ($include) {
        $parsed_logs[] = $log;
      }
    }
  }
  
  // Sort by timestamp (newest first)
  usort($parsed_logs, function($a, $b) {
    return $b['timestamp'] - $a['timestamp'];
  });
  
  // Pagination
  $page = isset($_GET['page']) ? (int)$_GET['page'] : 0;
  $limit = 50;
  $offset = $page * $limit;
  $total = count($parsed_logs);
  $paged_logs = array_slice($parsed_logs, $offset, $limit);
  
  if (empty($paged_logs)) {
    $build['empty'] = array(
      '#markup' => '<p>' . t('No log entries found matching the current filters.') . '</p>',
    );
    return $build;
  }
  
  // Add summary
  $build['summary'] = array(
    '#markup' => '<div class="xtd-mail-logger-summary">' . 
      t('Showing @filtered entries (@total total lines in file)', 
        array('@filtered' => count($paged_logs), '@total' => number_format($total_lines))) . 
      '</div>',
  );
  
  // Build table
  $header = array(
    t('Line #'),
    t('Timestamp'),
    t('Status'),
    t('Module/Key'),
    t('Subject'),
    t('From'),
    t('To'),
    t('Failure Reason'),
  );
  
  $rows = array();
  foreach ($paged_logs as $log) {
    $status_class = $log['status'] == 'PASS' ? 'status-pass' : 'status-fail';
    
    $rows[] = array(
      'data' => array(
        $log['line_number'],
        format_date($log['timestamp'], 'short'),
        array('data' => $log['status'], 'class' => array($status_class)),
        check_plain(isset($log['module_key']) ? $log['module_key'] : 'unknown'),
        array('data' => check_plain($log['subject']), 'style' => 'max-width: 200px; word-wrap: break-word;'),
        check_plain($log['from_email']),
        check_plain($log['to_email']),
        array('data' => check_plain($log['failure_reason']), 'style' => 'max-width: 300px; word-wrap: break-word;'),
      ),
    );
  }
  
  $build['table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('class' => array('xtd-mail-logger-table')),
  );
  
  // Manual pager for file logs
  if ($total > $limit) {
    global $pager_total, $pager_page_array;
    $element = 0;
    $pager_total[$element] = ceil($total / $limit);
    $pager_page_array = array($element => $page);
    
    $build['pager'] = array(
      '#theme' => 'pager',
    );
  }
  
  // Add file info
  $file_size = filesize($log_file);
  $file_modified = filemtime($log_file);
  
  $build['file_info'] = array(
    '#markup' => '<div class="xtd-mail-logger-file-info">' . 
      t('Log file: @file (@size bytes, last modified: @modified)', array(
        '@file' => $log_file,
        '@size' => number_format($file_size),
        '@modified' => format_date($file_modified, 'short'),
      )) . 
      '</div>',
  );
  
  // Add CSS
  drupal_add_css(drupal_get_path('module', 'xtd_mail_logger') . '/xtd_mail_logger.css');
  
  return $build;
}
