var url = "http://192.168.0.201:8008/ee128lab/mini_project/apis/";

var lock = new PatternLock('#my-pattern-lock',
    {
      matrix:[5,8],
      delimiter:"-",
      lineOnMove:true,
      enableSetPattern:true,
      onDraw:function(pattern){
        console.log((PWDStringResolve(lock.getPattern())));
        $.get(url + 'pwd.php?pwd=' + PWDStringResolve(lock.getPattern()),function (data, status) {
          if (status == "success") {
            console.log(data);
          }
        });
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

function PWDStringResolve(myStr) {
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

$("#btn-delay-time").click(function() {
  autoLockTime = $("#auto-lock-time-input").val();
  $.get(url + 'lock_delay.php?delay_time=' + autoLockTime.toString(), function (data, status) {
    if (status == 'success') {
      console.log(data);
    }
  });
});

$("#btn-relay").click(function () {
  $.get(url + 'relay.php',function (data, status) {
    if (status == "success") {
      console.log(data);
    }
  });
});

$("#btn-tem").click(function () {
  $.get(url + 'tem.php',function (data, status) {
    if (status == "success") {
      window.alert(data);//display the temperatureData on using alert. stupid!!!
    }
  });
});
