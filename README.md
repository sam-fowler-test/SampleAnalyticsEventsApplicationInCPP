
# AppDynamics-Cpp-Analytics-Example
A CPP example code on how to send data to the custom analytics schema using the Analytics API

## Steps:
1. Clone this repo
2. Install the cpprestsdk as provided by Microsoft: www.github.com/Microsoft/cpprestsdk (tested with 2.10.12.1 via NuGet installer through VS)
3. Create your schema
4. Update the following variables in the SampleAnalyticsEventsApplicationInCPP.cpp main class:

```
std::string analyticsUrl = "https://fra-ana-api.saas.appdynamics.com/events/publish/analytics_orders"; // Analytics Endpoint URL & Schema Name
std::string analyticsApiKey = "API KEY HERE"; // API Authentication Key
std::string analyticsGlobalAcc = "GLOBAL ACC NAME"; // Global Account Name (from License page)
```

5. Build and run

# Expected output
```
JSON Payload: [{"amount":80,"assets":"Name","order_date":"2020-11-10T09:33:38-0000","price":86.540000915527344,"subscribers":20}]
Status Code From Request: 200
Response Body From Request: 1
Call Successful, Response Body: null
```
# Reference
https://docs.appdynamics.com/display/PRO45/Analytics+Events+API
