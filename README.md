# XTD Mail Logger

A lightweight Drupal 7 module that logs all outgoing emails (both successful and failed) to files with detailed error information and email source tracking.

## Files Created

The module consists of the following files:

1. **xtd_mail_logger.info** - Module definition file
2. **xtd_mail_logger.module** - Main module with hooks and logging functions
3. **xtd_mail_logger.install** - Install/uninstall hooks and system requirements
4. **xtd_mail_logger.admin.inc** - Configuration form
5. **xtd_mail_logger.pages.inc** - Log viewing pages with filtering
6. **xtd_mail_logger.css** - Styling for the log tables
7. **README.md** - This documentation file

## Key Features

✅ **Automatic Integration**: Seamlessly hooks into Drupal's `drupal_mail()` system  
✅ **File-Based Logging**: Reliable logging to `sites/default/files/xtd_mail_logger.log`  
✅ **Email Source Tracking**: Shows which module/key sent each email (e.g., `user/register_admin_created`, `rules/ticket_notification`)  
✅ **Detailed Error Information**: Captures specific failure reasons instead of generic errors (when possible) 
✅ **Pattern Recognition**: Easy identification of which email types are failing  
✅ **Advanced Filtering**: Filter logs by status (PASS/FAIL), date range, and search across all fields  
✅ **Configurable Retention**: Auto-cleanup with customizable retention period (default 3 days)  
✅ **Lightweight**: No database overhead - file-only logging for maximum reliability  
✅ **User-Friendly Interface**: Clean admin interface with sorting, filtering, and pagination  
✅ **Manual Cleanup**: "Clear All Logs" button for immediate cleanup  
✅ **System Integration**: Proper Drupal permissions and status report integration

## Installation Instructions

### Step 1: Download and Place Files
1. Create the module directory: `sites/all/modules/custom/xtd_mail_logger/`
2. Download all module files and place them in the created directory
3. Ensure the files directory exists and is writable: `sites/default/files/`

### Step 2: Enable the Module
1. Navigate to **Admin → Modules** (`admin/modules`)
2. Find "XTD Mail Logger" in the Development section
3. Check the box next to the module name
4. Click "Save configuration"

### Step 3: Configure Permissions
1. Navigate to **Admin → People → Permissions** (`admin/people/permissions`)
2. Locate the "XTD Mail Logger" section
3. Assign permissions as needed:
   - **Administer XTD Mail Logger**: Allows users to configure module settings
   - **View XTD Mail Logs**: Allows users to view email logs

### Step 4: Configure the Module
1. Navigate to **Admin → Configuration → Development → XTD Mail Logger** (`admin/config/development/xtd-mail-logger`)
2. Configure your settings:
   - **Enable email logging**: Turn logging on/off
   - **Log retention period**: Set how many days to keep logs (0 = forever)
3. View current log statistics and file information
4. Use "Clear all logs" button if needed
5. Save configuration

### Step 5: View Logs
1. Navigate to **Admin → Reports → XTD Mail Logs** (`admin/reports/xtd-mail-logs`)
2. Use the filtering options to find specific logs:
   - **Filter by status**: Show only PASS or FAIL entries
   - **Date range**: Filter by specific date ranges
   - **Search**: Find logs containing specific text in any field

## Usage

### Automatic Logging
Once installed and enabled, the module automatically logs all emails sent through Drupal's `drupal_mail()` function. No additional configuration is required for basic logging functionality.

### Understanding Log Entries
Each log entry contains:
- **Timestamp**: When the email was sent
- **Status**: PASS (successful) or FAIL (failed)
- **Module/Key**: Which Drupal module and email type sent it
- **Subject**: The email subject line
- **From**: Sender email address
- **To**: Recipient email address
- **Failure Reason**: Specific error details for failed emails (empty for successful emails)

### Common Module/Key Patterns
- `user/register_admin_created` - New user welcome emails
- `user/password_reset` - Password reset emails
- `rules/[rule_name]` - Emails sent by Rules module
- `contact/page_mail` - Contact form emails
- `system/[various]` - System-generated emails

### Log Management
- **Automatic Cleanup**: Logs older than the configured retention period are automatically removed during cron runs
- **Manual Cleanup**: Use the "Clear all logs" button in configuration to immediately remove all logs
- **Log Statistics**: View current log counts and file size in the configuration page

### Filtering and Search
- **Status Filter**: Show only successful (PASS) or failed (FAIL) emails
- **Date Range**: Filter by from/to dates (YYYY-MM-DD format)
- **Search**: Find logs containing specific text in:
  - Module/Key (e.g., search "user" to find all user-related emails)
  - Subject line
  - From/To email addresses
  - Failure reasons

### Troubleshooting Email Issues
1. **Identify Patterns**: Look at the Module/Key column to see which types of emails are failing
2. **Check Failure Reasons**: Read the specific error messages in the Failure Reason column
3. **Search for Specific Issues**: Use the search box to find emails with specific error patterns
4. **Monitor Over Time**: Use date filters to see if issues are recent or ongoing

## Technical Details

### File Logging Format
Log entries in the file follow this format:
```
[YYYY-MM-DD HH:MM:SS] STATUS | Module: module/key | Subject: SUBJECT | From: FROM_EMAIL | To: TO_EMAIL | Reason: FAILURE_REASON
```

Example:
```
[2024-01-15 14:30:25] FAIL | Module: rules/ticket_notification | Subject: New Ticket #1234 | From: noreply@example.com | To: user@example.com | Reason: SMTP connection failed
```

### Cron Integration
The module integrates with Drupal's cron system to automatically clean up old logs:
- Runs during each cron execution
- Removes log entries older than the configured retention period
- Can be disabled by setting retention period to 0
- Manual cleanup always available via configuration page

### Error Detection
The module captures various types of email failures:
- PHP mail() function errors
- SMTP connection issues
- Invalid email addresses
- Missing required fields
- Server configuration problems
- Mail server response errors

### System Requirements
- Drupal 7.x
- Writable files directory (`sites/default/files/`)
- PHP with file system write permissions
- Cron configured for automatic log cleanup (optional)

## File Structure and Permissions

### Required Directories
- `sites/default/files/` - Must be writable by the web server

### Log File Location
- `sites/default/files/xtd_mail_logger.log` - Created automatically when first email is sent

### Permissions
The module checks directory permissions and displays warnings in the Drupal status report if issues are detected.

## Troubleshooting

### Common Issues

**Emails not being logged:**
- Verify the module is enabled
- Check that `sites/default/files/` is writable
- Ensure emails are sent through `drupal_mail()` function
- Check module configuration is enabled

**Log file not created:**
- Verify directory permissions on `sites/default/files/`
- Check Drupal status report for warnings
- Try sending a test email

**Log viewing page shows errors:**
- Clear Drupal caches
- Check file permissions
- Verify log file format is correct

### Status Report Integration
Check **Admin → Reports → Status report** for:
- Directory writability warnings
- Module configuration status  
- Log file statistics and information

### Getting Help
1. Check Drupal's watchdog logs at **Admin → Reports → Recent log messages**
2. Verify file permissions and directory access
3. Review the module's status in the Drupal status report
4. Check the configuration page for any error messages

## Benefits

### For Site Administrators
- **Quick Problem Identification**: Instantly see which emails are failing and why
- **Pattern Recognition**: Identify systematic issues affecting specific email types
- **Minimal Performance Impact**: File-based logging with no database overhead
- **Easy Maintenance**: Automated cleanup with configurable retention

### For Developers
- **Debugging Tool**: Detailed error messages help identify code issues
- **Integration Testing**: Verify email functionality across different modules
- **Performance Monitoring**: Track email system reliability over time
- **Module Identification**: See exactly which modules are sending emails

### For Users
- **Reliable Email Delivery**: Helps administrators ensure emails reach users
- **Issue Resolution**: Problems can be identified and fixed quickly
- **Transparency**: Clear logging of all email activity

## License

This module follows Drupal's licensing. Please refer to Drupal.org for specific licensing information.
