const express = require('express');
const path = require('path');
const fs = require('fs');
const app = express();
const port = process.env.PORT || 8888;


app.listen(port, function(){
  console.log(`Listening on port! ${port}`)
});


let buttonState = {
	degrees: 25,
	fanSpeeds: ['Auto','Min','Low','Medium','High','Max'],
	fanSpeed: 'Auto',
	modeType: 'Heat',
	modeTypes: ['Auto','Cool','Heat','Dry','Fan'],
	on: false,
	humidity: 100,
	temperature: 100,
	auto: false,
	maxTemperature: 22,
	minTemperature: 19
}
	
let autoInterval;
let response="";

app.get('/get/state',(req,res)=>{

	return res.json(buttonState);
});

app.get('/send/min-max/:minTemperature/:maxTemperature',(req,res)=>{
	buttonState.minTemperature = Number(req.params.minTemperature);
	buttonState.maxTemperature = Number(req.params.maxTemperature);
	return res.send("min-max send!");
});

app.get('/send/:command',(req,res)=>{
	checkState(req.params.command)
	return res.send("command send!");
});

app.get('/get/button/state/:degrees/:fanSpeed/:modeType/:on/:auto/:humidity/:temperature',(req,res)=>{
	buttonState.degrees = Number(req.params.degrees);
	buttonState.fanSpeed = req.params.fanSpeed;
	buttonState.modeType = req.params.modeType;
	buttonState.on = req.params.on === 'true';
	buttonState.auto = req.params.auto === 'true';
	buttonState.temperature = req.params.temperature;
	buttonState.humidity = req.params.humidity;
	return res.send("Button states OK!");
});

app.get('/get/command',(req,res)=>{
	const sendRes = res.send(response);
	response = null;
	return sendRes;
});

app.get('/',(req,res)=>{
	const index = `${__dirname}/index.html`
	return res.sendFile(index);
});

			
function setAuto(){
	clearInterval(autoInterval);
	autoInterval = setInterval(()=>{
		if(Number(buttonState.temperature) >=Number(buttonState.maxTemperature)){
				
			if(!buttonState.on) {
				buttonState.on = true;
				response = 'ON';
			}
		} else if (Number(buttonState.temperature) <=Number(buttonState.minTemperature)){
			if(buttonState.on) {
				buttonState.on = false;
				response = 'OFF';
			}
		}
			reloadState();
		},15000);
}

function checkState(state){
	switch (state){
		case "AUTO1":
			buttonState.auto = true;
			setAuto();
			response = `auto:ON`;
		break;
		case "AUTO0":
			buttonState.auto = false;
			response = `auto:OFF`;
		break;
		case "ON":
			buttonState.on = true;
			response = 'ON';
		break;
		case "OFF":
			buttonState.on = false;
			response = 'OFF';
		break;
		case "+":
			if (buttonState.degrees <30) {
				buttonState.degrees++;
			}
			response = `degrees:${buttonState.degrees}`;
		break;
		case "-":
			if (buttonState.degrees >16) {
				buttonState.degrees--;
			}
			response = `degrees:${buttonState.degrees}`;
		break;
		case "MODE":	
			buttonState.modeType = buttonState.modeTypes[(buttonState.modeTypes.indexOf(buttonState.modeType)+1) % buttonState.modeTypes.length];
			response = `MODE:${buttonState.modeType}`;
		break;
		case "FAN":
			buttonState.fanSpeed = buttonState.fanSpeeds[(buttonState.fanSpeeds.indexOf(buttonState.fanSpeed)+1 )% buttonState.fanSpeeds.length];
		response = `FAN:${buttonState.fanSpeed}`;
		break;
		
	}
}