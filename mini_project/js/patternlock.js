var lock = new PatternLock('#my-pattern-lock',
    {
      matrix:[5,8],
      delimiter:"-",
      lineOnMove:true,
      enableSetPattern:true,
      onDraw:function(pattern){
        console.log((stringResolve(lock.getPattern())));
        console.log(first(lock.getPattern()));
      }
});

function first(myStr) {
  var originList = myStr.split("-");
  var row = Math.floor((originList[0] - 1) / 8);
  var column = (originList[0] - 1) % 8;
  var data = {};
  data.x = column;
  data.y = row;
  return data;
}

function stringResolve(myStr) {
  var originList = myStr.split("-");
  var corList = [];
  for (var i = 0; i < originList.length; i++) {
    var row = Math.floor((originList[i] - 1) / 8);
    var column = (originList[i] - 1) % 8;
    var data = {};
    data.x = column;
    data.y = row;
    corList.push(data);
  }
  var retValues = [];
  for (var i = 0; i < 5; i++) {
    retValues.push(0x00);
  }
  for (var i = 0; i < corList.length; i++) {
    retValues[corList[i].y] += (0x80 >> corList[i].x);
  }
  var answers = [];
  for (var i = 0; i < retValues.length; i++) {
    var tmp = retValues[i].toString(16);
    if (tmp.length == 1) {
      tmp = '0' + tmp;
    }
    answers.push(tmp);
  }
  answers = answers.join('');
  var data = first(myStr);
  data.x = 7 - data.x;
  return data.x.toString() + data.y.toString() + answers;
}