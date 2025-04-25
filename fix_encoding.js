const fs = require('fs');
const path = require('path');
const iconv = require('iconv-lite');

// 娑擄拷閺傚洦鏁為柌濠傦拷鍦弾鐞涳拷
const commentMap = {
  // 1_simple_server.js
  '閿熸枻鎷烽敓绉哥鎷烽敓鏂ゆ嫹閿熸枻鎷�': '閹恒儲鏁归崚鐗堟殶閹癸拷',
  '閿熸枻鎷烽敓鏂ゆ嫹,': '閸ョ偛锟斤拷,',
  '閿熻緝鍖℃嫹閿熸枻鎷烽敓鏂ゆ嫹': '閺傦拷瀵偓鏉╃偞甯�',
  
  // 2_walk_server.js & 3_walk_server.js
  '閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�': '閺傛媽绻涢幒锟�',
  '閿熸枻鎷烽敓鏂ゆ嫹浣嶉敓鏂ゆ嫹': '閺囧瓨鏌婃担宥囩枂',
  '閿熷鎾�': '楠炴寧鎸�',
  
  // 3_chat_server.js
  '閿熺Ц纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹': '閺€璺哄煂濞戝牊浼�'
};

// 鐟曚礁锟藉嫮鎮婇惃鍕瀮娴犺泛鍨悰锟�
const files = [
  'Chapter1/1_simple_server.js',
  'Chapter1/2_walk_server.js',
  'Chapter1/3_chat_server.js',
  'Chapter1/3_walk_server.js'
];

// 婢跺嫮鎮婂В蹇庨嚋閺傚洣娆�
files.forEach(filePath => {
  try {
    // 鐠囪褰囬弬鍥︽閸愬懎锟斤拷
    const content = fs.readFileSync(filePath, 'utf8');
    
    // 閺囨寧宕查幍鈧張澶夎厬閺傚洣璐￠惍锟�
    let newContent = content;
    for (const [garbled, chinese] of Object.entries(commentMap)) {
      newContent = newContent.replace(new RegExp(garbled, 'g'), chinese);
    }
    
    // 閸愭瑥娲栭弬鍥︽
    fs.writeFileSync(filePath, newContent, 'utf8');
    console.log(`瀹歌弓鎱ㄦ径锟�: ${filePath}`);
  } catch (err) {
    console.error(`婢跺嫮鎮� ${filePath} 閺冭泛鍤柨锟�:`, err);
  }
});

console.log('閹碘偓閺堝鏋冩禒璺猴拷鍕倞鐎瑰本鍨�!'); 