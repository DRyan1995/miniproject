var lock = new PatternLock('#my-pattern-lock',{matrix:[5,8],delimiter:"-",
      onDraw:function(pattern){
        console.log(lock.getPattern());
      }
});
