  const SECOND = 1;
  const MINUTE = 60 * SECOND;
  const HOUR = 60 * MINUTE;
  const DAY = 24 * HOUR;
  const WEEK = 7 * DAY;
  var offset = 1*WEEK; // how far into the future to get events
  
  var calendarName = 'Bindicator'; // update with your own Google calendar name


function doGet() {
    var cal = CalendarApp.getCalendarsByName(calendarName)[0];
     
  if (!cal) {
    return {status: 'error', message: 'Error, calendar "' + calendarName + '" does not exist.'};
  }
    var now = new Date(),
      later = new Date();
  
  later.setSeconds(now.getSeconds() + offset);
  
  var items = cal.getEvents(now, later),
      length = items.length;
   if (length){ 
    var data = ""; //length + " items found\n";
    for (x=0; x<length; x++)
    { if (x>0) data += ",";
      data += getEvent(items[x]) + "\n";
    
    }
  return ContentService.createTextOutput("["+data+"]");
   }
   else {
   return ContentService.createTextOutput("NOTHING FOUND");
   }
}
function getEvent(item) {
  var now = new Date(),
      title = item.getTitle(),
      startTime = item.getStartTime(),
      endTime = item.getEndTime(),
      fromNow = startTime - now,
      startsWithin = fromNow < 0 ? 0 : Math.round((fromNow)/1000),
      lasts = Math.round((endTime - (fromNow < 0 ? now : startTime))/1000);
      response = "{'" + title + "' : '"+ Utilities.formatDate(startTime, "GMT+9.5", "dd/MM/yyyy") + "'}";
  return response;
} 

function getOutput(calendarName, offset) {
  return JSON.stringify(getResponse(calendarName, offset));
}

function getResponse(calendarName, offset) {
  var cal = CalendarApp.getCalendarsByName(calendarName)[0];
  
  if (!cal) {
    return {status: 'error', message: 'Error, calendar "' + calendarName + '" does not exist.'};
  }
  
  var now = new Date(),
      later = new Date();
  
  later.setSeconds(now.getSeconds() + offset);
  
  var items = cal.getEvents(now, later),
      length = items.length,
      response = {};
  
  if (length){
    response = getEvent(items[0]);
    if (response.nextCheckIn > offset) response.nextCheckIn = offset;
  } else {
    response = {closeValve: true, nextCheckIn: offset};
  }
  
  return {status: 'success', data: '['+response+']'};
}

