/**
 * Handles HTTP GET requests to fetch Google Calendar events for the CURRENT DAY.
 * Returns events (title, start time HH:mm, end time HH:mm, location) as a JSON string.
 */
function doGet(e) {
  // --- Configuration ---
  const calendarId = 'primary'; // Use 'primary' for the main calendar, or the specific calendar ID email address
  // ---------------------

  try {
    // Get the calendar and its timezone
    var calendar = CalendarApp.getCalendarById(calendarId);
    if (!calendar) {
      return ContentService.createTextOutput(JSON.stringify({ error: 'Calendar not found: ' + calendarId })).setMimeType(ContentService.MimeType.JSON);
    }
    var timeZone = calendar.getTimeZone(); // Get the calendar's timezone for correct date formatting

    // Calculate start and end of the current day in the calendar's timezone
    var now = new Date();
    var startOfDay = new Date(now.getFullYear(), now.getMonth(), now.getDate(), 0, 0, 0, 0);
    // Set time to midnight start in the script's default timezone, then format using calendar's timezone below

    var endOfDay = new Date(now.getFullYear(), now.getMonth(), now.getDate() + 1, 0, 0, 0, 0); // Midnight of the *next* day (exclusive)
    // Alternatively: new Date(now.getFullYear(), now.getMonth(), now.getDate(), 23, 59, 59, 999);


    Logger.log("Querying events from: " + Utilities.formatDate(startOfDay, timeZone, "yyyy-MM-dd HH:mm:ss") +
               " to " + Utilities.formatDate(endOfDay, timeZone, "yyyy-MM-dd HH:mm:ss") +
               " in timezone " + timeZone);

    // Get events for today
    var events = calendar.getEvents(startOfDay, endOfDay);
    Logger.log("Found " + events.length + " raw events for today.");

    // Prepare the data to send back
    var eventData = [];
    for (var i = 0; i < events.length; i++) {
      var event = events[i];
      var eventStartTime = event.getStartTime();
      var eventEndTime = event.getEndTime();

      // Optional: Skip all-day events more reliably
      if (event.isAllDayEvent()) {
         Logger.log("Skipping all-day event: " + event.getTitle());
         continue;
      }
      // Optional: Check if the event strictly starts today (useful if getEvents includes overlapping events from yesterday)
      // This check might be redundant depending on how getEvents handles exact boundaries, but can add robustness.
      // if (eventStartTime < startOfDay) {
      //   continue; // Skip events that started before today
      // }


      var location = event.getLocation() || ""; // Get location, default to empty string if none

      eventData.push({
        title: event.getTitle(),
        // Format times directly into HH:mm using the calendar's timezone
        startTime: Utilities.formatDate(eventStartTime, timeZone, "HH:mm"),
        endTime: Utilities.formatDate(eventEndTime, timeZone, "HH:mm"),
        location: location
        // Keep original ISO strings if needed for other purposes (optional)
        // startISO: eventStartTime.toISOString(),
        // endISO: eventEndTime.toISOString()
      });
    }
     Logger.log("Prepared " + eventData.length + " events to return.");

    // Return the data as JSON
    return ContentService.createTextOutput(JSON.stringify({ events: eventData }))
           .setMimeType(ContentService.MimeType.JSON);

  } catch (error) {
    // Log the error for debugging in Apps Script console
    Logger.log("Error in doGet: " + error + " Stack: " + error.stack);
    // Return an error message as JSON
    return ContentService.createTextOutput(JSON.stringify({ error: 'Script execution failed: ' + error.toString() }))
           .setMimeType(ContentService.MimeType.JSON);
  }
}

// Helper function to test within Apps Script editor
function testGetTodaysEvents() {
  var fakeEvent = { // Simulate a request object if your script uses parameters from 'e' (this one doesn't)
      parameter: {},
      contextPath: "",
      contentLength: -1,
      queryString: "",
      parameters: {}
  };
  var result = doGet(fakeEvent); // Call the main function
  Logger.log("Test Output Type: " + typeof result);
  if (result && typeof result.getContent === 'function') {
       Logger.log(result.getContent()); // Log the JSON output
  } else {
       Logger.log("Test function did not return a ContentService object.");
       Logger.log(result); // Log whatever was returned
  }

   // Log current time interpretation for debugging timezone issues
   var now = new Date();
   var calendar = CalendarApp.getCalendarById('primary'); // Or your specific ID
   var timeZone = calendar.getTimeZone();
   Logger.log("Current time (script default timezone): " + now);
   Logger.log("Current time (calendar timezone " + timeZone + "): " + Utilities.formatDate(now, timeZone, "yyyy-MM-dd HH:mm:ss Z"));
   var startOfDay = new Date(now.getFullYear(), now.getMonth(), now.getDate(), 0, 0, 0, 0);
   var endOfDay = new Date(now.getFullYear(), now.getMonth(), now.getDate() + 1, 0, 0, 0, 0);
   Logger.log("Calculated Start of Day (calendar timezone): " + Utilities.formatDate(startOfDay, timeZone, "yyyy-MM-dd HH:mm:ss Z"));
   Logger.log("Calculated End of Day (calendar timezone): " + Utilities.formatDate(endOfDay, timeZone, "yyyy-MM-dd HH:mm:ss Z"));

}