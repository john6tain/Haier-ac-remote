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
	on:true,
	humidity: 10,
	temperature: 25,
	auto:false,
}
	

let response="";

app.get('/get/state',(req,res)=>{

	return res.json(buttonState);
});


app.get('/send/:command',(req,res)=>{
	checkState(req.params.command)
	//console.log(req.params.command);
	//console.log(response);
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

function checkState(state){
	switch (state){
		case "AUTO1":
			buttonState.auto = true;
		break;
		case "AUTO0":
			buttonState.auto = false;
		break;
		case "ON":
			buttonState.on = false;
			response = 'ON';
		break;
		case "OFF":
			buttonState.on = true;
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