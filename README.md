# XTD Mail Logger

A Drupal 7 module that logs all outgoing emails (both successful and failed) to database and/or files with configurable options and a management interface.

Note that database logging is untested! The file logging is working well for us.

By Brian Reed using Claude.ai (9/13/2025)

## Files

The module consists of the following files:

1. **xtd_mail_logger.info** - Module definition file
2. **xtd_mail_logger.module** - Main module with hooks and logging functions
3. **xtd_mail_logger.install** - Database schema and install/uninstall hooks
4. **xtd_mail_logger.admin.inc** - Configuration form
5. **xtd_mail_logger.pages.inc** - Log viewing pages with filtering
6. **xtd_mail_logger.css** - Styling for the log tables
7. **README.md** - This documentation file

## Key Features

✅ **Drupal Integration**: Automatically hooks into Drupal's `drupal_mail()` system via `hook_mail_alter()`  
✅ **Dual Logging Options**: Configurable logging to both database and file (`sites/default/files/xtd_email_logger.log`)  
✅ **Complete Email Data**: Logs timestamp, PASS/FAIL status, failure reason, subject, from address, and to address  
✅ **Configuration Page**: Easy-to-use admin interface at Admin → Configuration → Development → XTD Mail Logger  
✅ **Separate Log Views**: Database logs and file logs displayed in separate tabs for better organization  
✅ **Advanced Filtering**: Filter logs by status (PASS/FAIL), date range, and search across all fields  
✅ **Table Sorting**: Sortable columns with pagination support  
✅ **Automatic Cleanup**: Cron job automatically removes logs older than 7 days  
✅ **Manual Cleanup**: "Clear All Logs" button on configuration page for immediate cleanup  
✅ **Proper Permissions**: Granular permissions for administration and log viewing  
✅ **Log Statistics**: Real-time display of current log counts on configuration page  
✅ **Error Handling**: Validates configuration settings and checks directory permissions

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
2. Choose your logging preferences:
   - **Log to database**: Store logs in the database for easy searching and filtering
   - **Log to file**: Store logs in `sites/default/files/xtd_email_logger.log`
3. View current log statistics
4. Save configuration

### Step 5: View Logs
1. Navigate to **Admin → Reports → XTD Mail Logs** (`admin/reports/xtd-mail-logs`)
2. Use the tabs to switch between:
   - **Database Logs**: Searchable, sortable database entries
   - **File Logs**: Raw log file entries with filtering

## Usage

### Automatic Logging
Once installed and enabled, the module automatically logs all emails sent through Drupal's `drupal_mail()` function. No additional configuration is required for basic logging functionality.

### Log Management
- **Automatic Cleanup**: Logs older than 7 days are automatically removed during cron runs
- **Manual Cleanup**: Use the "Clear all logs" button on the configuration page to immediately remove all logs
- **Log Statistics**: View current log counts on the configuration page

### Filtering and Search
On both database and file log pages, you can:
- Filter by email status (PASS/FAIL)
- Filter by date range (from/to dates)
- Search across subject, from address, to address, and failure reason fields
- Sort by any column (database logs only)
- Navigate through paginated results

## Technical Details

### Database Schema
The module creates a `xtd_mail_logger` table with the following fields:
- `id`: Primary key (serial)
- `timestamp`: Unix timestamp of when email was sent
- `status`: Email status (PASS or FAIL)
- `failure_reason`: Reason for failure (if applicable)
- `subject`: Email subject line
- `from_email`: Sender email address
- `to_email`: Recipient email address

### File Logging Format
Log entries in the file follow this format:
```
[YYYY-MM-DD HH:MM:SS] STATUS | Subject: SUBJECT | From: FROM_EMAIL | To: TO_EMAIL | Reason: FAILURE_REASON
```

### Cron Integration
The module integrates with Drupal's cron system to automatically clean up old logs. The cleanup process:
- Runs during each cron execution
- Removes database entries older than 7 days
- Removes file log entries older than 7 days
- Can be triggered manually via Drupal's cron interface

## Troubleshooting

### Common Issues

**File logging not working:**
- Ensure `sites/default/files/` directory is writable by the web server
- Check the Status Report at Admin → Reports → Status report for file system warnings

**No emails being logged:**
- Verify the module is enabled
- Ensure emails are being sent through `drupal_mail()` function
- Check if logging is enabled in the module configuration

**Permission denied errors:**
- Verify user has appropriate permissions assigned
- Check that the module's permissions are properly configured

### Support
For issues specific to this module, check:
1. Drupal's watchdog logs at Admin → Reports → Recent log messages
2. Web server error logs
3. The module's configuration page for status indicators

## Requirements

- Drupal 7.x
- MySQL or MariaDB database
- Writable files directory (`sites/default/files/`)
- PHP with file system write permissions

## License

This module follows Drupal's licensing. Please refer to Drupal.org for specific licensing information.
