//________________________________________________________________________________
// Google Sheets and Google Apps Script Project Information.

/*
Google Sheets Project Name      : ESP32_Google_Spreadsheet
Google Sheets ID                : -- Ask Me For ID -- 
Sheet Name                      : ESP32_Google_Sheets_Sheet

Modified by:    Anthony Le
Date Modified:  11/06/2024
Credit: https://www.youtube.com/watch?v=u7TYu61l0t4&t=144s

Fingerprint Sensor Logger                    
Date | Time | ID | Image | Person  | Authorization

Latest Fingerprint Scanner Data                  
Date | Time | ID | Image | Person  | Authorization

Google Apps Script Project Name : ESP32_Google_Spreadsheet_Apps_Script
Web app URL                     : 

Web app URL Test Write : ?sts=write&id=2&image=5&person=Bob&authorization=Yes
Web app URL Test Read  : ?sts=read
*/

//________________________________________________________________________________
// Google Sheets and Google Apps Script Project Information.
// Project Name      : ESP32_Google_Spreadsheet
// Google Sheets ID  : -- Ask Me For ID -- 
// Sheet Name        : ESP32_Google_Sheets_Sheet
//
// This script logs fingerprint sensor data received from an ESP32 device to Google Sheets.
//________________________________________________________________________________
//________________________________________________________________________________
function doGet(e) { 
  Logger.log("Received parameters: " + JSON.stringify(e)); // Log incoming parameters for debugging
  
  var result = 'Ok';

  // Check if parameters are provided
  if (!e.parameter || Object.keys(e.parameter).length === 0) {
    result = 'No Parameters';
    return ContentService.createTextOutput(result);
  }

  // Google Sheets configuration
  var sheet_id = '-- Hidden --';                                // Spreadsheet ID *note ask me for sheet_id
  var sheet_name = "ESP32_Google_Sheets_Sheet";                 // Sheet Name

  // Open the spreadsheet and check for sheet existence
  var sheet_open = SpreadsheetApp.openById(sheet_id);
  var sheet_target = sheet_open.getSheetByName(sheet_name);
  if (!sheet_target) {
    result = "Error: Sheet not found";
    return ContentService.createTextOutput(result);
  }

  // Prepare data for logging
  var newRow = sheet_target.getLastRow() + 1;
  var rowDataLog = [];
  var Data_for_H3, Data_for_I3, Data_for_J3, Data_for_K3, Data_for_L3, Data_for_M3;

  // Get current date and time
  var Curr_Date = Utilities.formatDate(new Date(), "UTC-8", 'dd/MM/yyyy');
  var Curr_Time = Utilities.formatDate(new Date(), "UTC-8", 'h:mma');
  
  // Assign date and time values
  rowDataLog[0] = Curr_Date; 
  rowDataLog[1] = Curr_Time; 
  Data_for_H3 = Curr_Date;
  Data_for_I3 = Curr_Time;

  // Initialize sts_val and log received parameter keys
  var sts_val = '';
  Logger.log('Parameters provided: ' + JSON.stringify(e.parameter));

  // Process incoming parameters and assign values
  for (var param in e.parameter) {
    var value = stripQuotes(e.parameter[param]);
    Logger.log(`Processing parameter ${param} with value ${value}`);
    
    switch (param) {
      case 'sts':
        sts_val = value;
        Logger.log('sts_val set to: ' + sts_val);
        break;

      case 'id':
        rowDataLog[2] = value;       // Column C
        Data_for_J3 = value;         // Latest data column J3
        result += ', ID Written on column C';
        break;

      case 'image':
        rowDataLog[3] = value;       // Column D
        Data_for_K3 = value;         // Latest data column K3
        result += ', Image Written on column D';
        break;

      case 'person':
        rowDataLog[4] = value;       // Column E
        Data_for_L3 = value;         // Latest data column L3
        result += ', Person Written on column E';
        break;

      case 'authorization':
        rowDataLog[5] = value;       // Column F
        Data_for_M3 = value;         // Latest data column M3
        result += ', Authorization Written on column F';
        break;

      default:
        result += ", unsupported parameter";
    }
  }

  // Check and execute based on sts_val
  Logger.log('Final sts_val: ' + sts_val);
  
  // Write data to Google Sheets if the sts status is "write"
  if (sts_val === 'write') {
    Logger.log("Writing data to sheet with data: " + JSON.stringify(rowDataLog));
    
    // Add data to the main logging section
    var newRangeDataLog = sheet_target.getRange(newRow, 1, 1, rowDataLog.length);
    newRangeDataLog.setValues([rowDataLog]);
    
    // Update the latest data section
    var RangeDataLatest = sheet_target.getRange('H3:M3');
    RangeDataLatest.setValues([[Data_for_H3, Data_for_I3, Data_for_J3, Data_for_K3, Data_for_L3, Data_for_M3]]);
    
    return ContentService.createTextOutput(result);
  }
  
  // Read data if the sts status is "read"
  if (sts_val === 'read') {      
    Logger.log("Reading data from latest data section (J3:M3).");
    var all_Data = sheet_target.getRange('J3:M3').getValues();
    Logger.log("Read data: " + JSON.stringify(all_Data));
    return ContentService.createTextOutput(JSON.stringify(all_Data[0])); // Return as a JSON array
  }

  // Fallback message if neither write nor read executed
  Logger.log("No valid operation specified (write or read).");
  return ContentService.createTextOutput("No valid operation specified (write or read).");
}

// Helper function to strip surrounding quotes from a string
function stripQuotes(value) {
  return value.replace(/^["']|['"]$/g, "");
}
//________________________________________________________________________________

