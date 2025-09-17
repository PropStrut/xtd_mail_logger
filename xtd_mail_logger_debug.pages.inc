<?php

/**
 * @file
 * Simplified debug version of page callbacks for XTD Mail Logger module.
 */

/**
 * Main logs page callback.
 */
function xtd_mail_logger_logs_page() {
  // Simple redirect to database logs
  drupal_goto('admin/reports/xtd-mail-logs/database');
}

/**
 * Database logs page callback.
 */
function xtd_mail_logger_database_logs_page() {
  $output = '';
  
  // Check if database logging is enabled
  if (!variable_get('xtd_mail_logger_database', TRUE)) {
    return '<p>' . t('Database logging is currently disabled. Enable it in the <a href="@config">configuration</a>.', 
      array('@config' => url('admin/config/development/xtd-mail-logger'))) . '</p>';
  }
  
  // Simple query without filters for debugging
  try {
    $query = db_select('xtd_mail_logger', 'xml');
    $query->fields('xml');
    $query->orderBy('timestamp', 'DESC');
    $query->range(0, 20); // Limit to 20 records for debugging
    
    $result = $query->execute();
    
    $header = array('Timestamp', 'Status', 'Subject', 'From', 'To', 'Failure Reason');
    $rows = array();
    
    foreach ($result as $log) {
      $rows[] = array(
        format_date($log->timestamp, 'short'),
        $log->status,
        check_plain($log->subject),
        check_plain($log->from_email),
        check_plain($log->to_email),
        check_plain($log->failure_reason),
      );
    }
    
    if (empty($rows)) {
      $output .= '<p>No log entries found in database.</p>';
      $output .= '<p>Table exists: ' . (db_table_exists('xtd_mail_logger') ? 'YES' : 'NO') . '</p>';
    } else {
      $output .= theme('table', array('header' => $header, 'rows' => $rows));
    }
    
  } catch (Exception $e) {
    $output .= '<p>Error querying database: ' . check_plain($e->getMessage()) . '</p>';
    $output .= '<p>Table exists: ' . (db_table_exists('xtd_mail_logger') ? 'YES' : 'NO') . '</p>';
  }
  
  return $output;
}

/**
 * File logs page callback.
 */
function xtd_mail_logger_file_logs_page() {
  $output = '';
  
  // Check if file logging is enabled
  if (!variable_get('xtd_mail_logger_file', TRUE)) {
    return '<p>' . t('File logging is currently disabled. Enable it in the <a href="@config">configuration</a>.', 
      array('@config' => url('admin/config/development/xtd-mail-logger'))) . '</p>';
  }
  
  $log_file = 'sites/default/files/xtd_email_logger.log';
  
  if (!file_exists($log_file)) {
    $output .= '<p>Log file does not exist: ' . $log_file . '</p>';
    $output .= '<p>Directory writable: ' . (is_writable('sites/default/files') ? 'YES' : 'NO') . '</p>';
    return $output;
  }
  
  try {
    $lines = file($log_file, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
    
    if (empty($lines)) {
      $output .= '<p>Log file is empty.</p>';
    } else {
      $output .= '<p>Found ' . count($lines) . ' lines in log file.</p>';
      
      // Show raw content first
      $output .= '<h3>Raw file content:</h3>';
      $output .= '<pre>' . check_plain(file_get_contents($log_file)) . '</pre>';
      
      // Try to parse each line
      $output .= '<h3>Parsing results:</h3>';
      $parsed_count = 0;
      
      foreach ($lines as $line_number => $line) {
        $line = trim($line);
        if (empty($line)) continue;
        
        $output .= '<p><strong>Line ' . ($line_number + 1) . ':</strong><br>';
        $output .= 'Raw: <code>' . check_plain($line) . '</code><br>';
        
        // Try the regex pattern
        if (preg_match('/^\[([^\]]+)\]\s+(\w+)\s*\|\s*Subject:\s*(.*?)\s*\|\s*From:\s*(.*?)\s*\|\s*To:\s*(.*?)\s*\|\s*Reason:\s*(.*)$/', $line, $matches)) {
          $parsed_count++;
          $output .= 'PARSED: Timestamp=' . $matches[1] . ', Status=' . $matches[2] . ', Subject=' . $matches[3] . '<br>';
        } else {
          $output .= '<span style="color: red;">FAILED TO PARSE</span><br>';
        }
        $output .= '</p>';
      }
      
      $output .= '<p><strong>Summary:</strong> ' . $parsed_count . ' out of ' . count($lines) . ' lines parsed successfully.</p>';
    }
    
  } catch (Exception $e) {
    $output .= '<p>Error reading log file: ' . check_plain($e->getMessage()) . '</p>';
  }
  
  return $output;
}
