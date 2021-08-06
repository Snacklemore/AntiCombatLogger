string ENDPOINT_URL = "default_Value";

class WebHook
{
	
	string endpointURL ;
	string JSONData;
	string JSONDataFooter;
	string name;
	string uid;
	string content = "content";
	string message_v =  "defTest";
	string footer  =  "footer";
	string text  = "text";
	string footMessage = "currentDate_default";
	void WebHook()
	{
		JSONData  =  "\""  + content  + "\":\"" + message_v + "\",";
		endpointURL = ENDPOINT_URL;
		
	}
	void UpdateURL()
	{
		endpointURL  = ENDPOINT_URL;
	}
	void  buildJSON()
	{
		JSONData  =  "\""  + content  + "\":\"" + message_v + "\",";
	}
	string getDate()
	{
		int year;
		int month;
		int day;
		GetYearMonthDay(year,month,day);
		
		int hour;
		int minute;
		int second;
		string date;
		GetHourMinuteSecond(hour,minute,second);
		if (minute < 10 )
		{
			string minuteString = "0"+minute.ToString() ;
			date = hour.ToString() + ":"  + minuteString + "   ||  " + day.ToString() +"." + month.ToString() + "." + year.ToString();

		}
		else 
		{
			date = hour.ToString() + ":"  + minute.ToString() + "   ||  " + day.ToString() +"." + month.ToString() + "." + year.ToString();

		}
			
		return date;
		
	
	}
	void buildJSONDataFooter()
	{
		JSONDataFooter  =  "\"embeds\":[ {\"footer\": {\""+text+"\": \""+footMessage+"\"}}]";
		
	}
	
	string wrap(string s)
	{
		s = "{" + s + "}";
		return s;
	}
	bool Send(string message,string footerMessage)
	{
		message_v = message;
		footMessage= getDate();
		buildJSON();
		buildJSONDataFooter();
		JSONData =  JSONData +  JSONDataFooter;
		JSONData  = wrap(JSONData);
		
		
		RestCallback rcb = new RESTCallbackQuiet;
		RestApi core = GetRestApi();
		
		if (!core)
			core  =  CreateRestApi();
		
		RestContext rctx =  core.GetRestContext(endpointURL);
		rctx.SetHeader("application/json");
		Print("Sending "+ JSONData  + " to adress " + endpointURL);
		rctx.POST(rcb,"",JSONData);
		return true;
			
		
		
	}
	
	void setMessage(string m)
	{
		message_v  = m;
	}
	void  setEndPointURL(string url)
	{
		endpointURL  =   url;
		
	}
}
	


static ref  WebHook g_webhook;
static ref  WebHook GetWebhook()
{
	if  (!g_webhook)
		{
			g_webhook  = new WebHook;
		
		}
	return  g_webhook;
	
}
class RESTCallbackQuiet  :  RestCallback
{
	override void  OnError(int  errorCode)  {Print("QuietCallBackError SHHHH!");}
	override void OnTimeout() {Print("QuietCallBackTimeOut SHHHH!");}
	override void OnSuccess(string data, int dataSize) {Print("QuietCallBackOnSuccess SHHHH!");}
	
	
}