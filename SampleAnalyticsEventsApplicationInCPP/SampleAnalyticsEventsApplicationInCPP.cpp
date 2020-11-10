// SampleAnalyticsEventsApplicationInCPP.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Requires the NuGet package cpprestsdk, tested with version 2.10.12.1 ~ www.github.com/Microsoft/cpprestsdk

#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <string>
#include <chrono>
#include <ctime>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;
using std::string;

// Creates an object to set the required payload (only setupt via a constructor, no getters/setters - have direct property access instead as they're public)
class AnalyticsPayload {
	public:
		float price;
		int amount;
		int subscribers;
		std::string assets;
		std::string currentTime;

		AnalyticsPayload(float price, int amount, int subscribers, std::string assets, std::string currentTime) {
			this->price = price;
			this->amount = amount;
			this->subscribers = subscribers;
			this->assets = assets;
			this->currentTime = currentTime;
		}
};

// Manually deserialize AnalyticsPayload Object to JSON ie.  [{"amount":80,"assets":"Name","order_date":"2020-11-09T17:03:54-0000","price":100,"subscribers":20}]
json::value buildJsonPayload(AnalyticsPayload analyticsPayload) { 
	//This could be done better using a deserializer package - wanted to keep it as vanilla as possible
	
	std::vector<json::value> jsonArray; // Array 
	
	json::value jsonObj; // key/value pairs for payload - to be added to array, no for loop as its a single entry in this instance
	jsonObj[U("price")] = json::value::number(analyticsPayload.price);
	jsonObj[U("amount")] = json::value::number(analyticsPayload.amount);
	jsonObj[U("assets")] = json::value::string(utility::conversions::to_string_t(analyticsPayload.assets));
	jsonObj[U("order_date")] = json::value::string(utility::conversions::to_string_t(analyticsPayload.currentTime));
	jsonObj[U("subscribers")] = json::value::number(analyticsPayload.subscribers);
	
	jsonArray.push_back(jsonObj); // add to array object

	json::value jsonPayload; //Declare final JSON payload object for return
	jsonPayload = json::value::array(jsonArray); // append array to ensure payload is sent as [{key:val,...}]

	std::wcout << "JSON Payload: " << jsonPayload << std::endl;

	return jsonPayload;
}

// POSTs the JSON Payload to the Analytics Endpoint using the Authorization Key/Account as Headers.
void makeAnalyticsRestRequest(std::string analyticsUrl, std::string analyticsKeyHeader, std::string analyticsAccHeader, json::value jsonPayload) {

	http_client client(utility::conversions::to_string_t(analyticsUrl));
	http_request request(methods::POST);

	// Set Headers
	request.headers().add(U("X-Events-API-Key"), utility::conversions::to_string_t(analyticsKeyHeader));
	request.headers().add(U("X-Events-API-AccountName"), utility::conversions::to_string_t(analyticsAccHeader));
	request.headers().add(U("Content-type"), U("application/vnd.appd.events+json;v=2"));
	request.headers().add(U("Accept"), U("application/vnd.appd.events+json;v=2"));

	// Set Payload
	request.set_body(jsonPayload);

	// Make the call and handle the response
	client.request(request)
		.then([](http_response response)
			{
				std::cout << "Status Code From Request: " << response.status_code() << std::endl;
				std::cout << "Response Body From Request: " << response.body() << std::endl;

				if (response.status_code() == status_codes::OK)
				{
					return response.extract_json();
				}
				return pplx::task_from_result(json::value());
			})
		.then([](pplx::task<json::value> previousTask)
			{
				try
				{
					//Print the results to console
					std::wcout << "Call Successful, Response Body: " << previousTask.get().serialize() << std::endl;
				}
				catch (http_exception const& e)
				{
					std::wcout << e.what() << std::endl;
				}
			})
		.wait();
}

// Gets the current time and converts to a common format (UTC) (copied from SO https://stackoverflow.com/questions/36190924/localtime-s-and-strftime-usage-building-iso-time-string)
std::string getCurrentTime() {
	auto time = std::chrono::system_clock::now();
	std::time_t rawTime = std::chrono::system_clock::to_time_t(time);
	struct tm timeinfo;
	localtime_s(&timeinfo, &rawTime);

	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch());
	std::size_t frac_seconds = ms.count() % 1000;

	char buffer[32];
	std::strftime(buffer, 32, "%Y-%m-%dT%H:%M:%S%z", &timeinfo);

	std::string bufferStr(buffer);

	return bufferStr;
}

int main() {

	// Set Connection Variables
	std::string analyticsUrl = "https://fra-ana-api.saas.appdynamics.com/events/publish/analytics_orders"; // Analytics Endpoint URL & Schema Name
	std::string analyticsApiKey = "API KEY HERE"; // API Authentication Key
	std::string analyticsGlobalAcc = "GLOBAL ACCOUNT HERE"; // Global Account Name (from License page)

	// Create Payload Object
	float price = 86.54; // integer variable of the 'Price' column
	int amount = 80; // integer variable of the 'Amount' column
	int subscribers = 20; // integer variable of the 'Subscribers' column
	std::string assets = "Name"; // String variable of the 'Assets' column
	std::string currentTime = getCurrentTime(); // String variable of the 'Current_Time' column

	AnalyticsPayload analyticsPayload(price,amount,subscribers,assets,currentTime); // Payload Object
	
	// Deserialise Payload Object To JSON
	json::value jsonPayload = buildJsonPayload(analyticsPayload);
	
	// Send HTTP Request
	makeAnalyticsRestRequest(analyticsUrl, analyticsApiKey, analyticsGlobalAcc, jsonPayload);

	return 0;
}
