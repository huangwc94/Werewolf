var say = require("say");
var fs = require('fs');
var utl = require("util");
var contents = fs.readFileSync('manifest.txt','utf8');

var lines = contents.split("\n");

for(var i in lines){
  var id = parseInt(lines[i].split(":")[0]);
  var text = lines[i].split(":")[1];
  filen = numFmt(id,"000")+".wav";
  console.log("creating voice "+filen+":" + text);
  say.export(text, 'Ting-Ting', 1.0, "output/" + filen, function(err) {
    if (err) {
      return console.error(err);
    }else{
      console.log('Text has been saved!');
    }
  });
};

function numFmt(num, mask) {
	return (mask + num).slice(-Math.max(mask.length, (num + "").length));
};
