const express = require('express');
const path = require('path');
const fs = require('fs');
const app = express();
const bodyParser = require('body-parser');
const port = process.env.PORT || 8888;


app.listen(port, function(){
  console.log(`Listening on port! ${port}`)
});

app.use(bodyParser({defer: true}));

let buttonState = {
	degrees: 25,
	fanSpeeds: ['Auto','Min','Low','Medium','High','Max'],
	fanSpeed: 'Auto',
	modeType: 'Heat',
	modeTypes: ['Auto','Cool','Heat','Dry','Fan'],
	on:true
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