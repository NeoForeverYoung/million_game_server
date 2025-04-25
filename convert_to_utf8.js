const fs = require('fs');
const path = require('path');
const iconv = require('iconv-lite');

// 鐎规矮绠熺憰浣瑰笓闂勩倗娈戦惄锟借ぐ鏇炴嫲閺傚洣娆�
const excludeDirs = ['.git', 'node_modules', 'build', 'dist'];
const excludeExtensions = ['.png', '.jpg', '.jpeg', '.gif', '.svg', '.ico', '.pdf', '.zip', '.tar', '.gz', '.exe', '.dll', '.so'];

// 鐟曚礁锟藉嫮鎮婇惃鍕�娲拌ぐ锟�
const rootDir = '.';

// 闁�鎺戠秺闁�宥呭坊閻╋拷瑜版洖锟藉嫮鎮婇弬鍥︽��
function processDirectory(directory) {
  const items = fs.readdirSync(directory);
  
  for (const item of items) {
    // 鐠哄疇绻冮幒鎺楁珟閻ㄥ嫮娲拌ぐ锟�
    if (excludeDirs.includes(item)) {
      continue;
    }
    
    const fullPath = path.join(directory, item);
    const stat = fs.statSync(fullPath);
    
    if (stat.isDirectory()) {
      // 闁�鎺戠秺婢跺嫮鎮婄€涙劗娲拌ぐ锟�
      processDirectory(fullPath);
    } else if (stat.isFile()) {
      // 濡�鈧�閺屻儲鏋冩禒鑸靛⒖鐏炴洖鎮�
      const ext = path.extname(fullPath).toLowerCase();
      if (excludeExtensions.includes(ext)) {
        continue;
      }
      
      try {
        // 鐠囪�插絿閺傚洣娆㈤崘鍛�锟界櫢绱欐禍宀冪箻閸掕埖鏌熷��蹇ョ礆
        const buffer = fs.readFileSync(fullPath);
        
        // 鐏忔繆鐦�濡�鈧�濞村��妲搁崥锕€瀵橀崥锟芥稉锟介弬鍥х摟缁楋缚绗栨稉宥嗘ЦUTF-8缂傛牜鐖�
        const utf8Content = buffer.toString('utf8');
        const hasChineseChar = /[\u4e00-\u9fa5]/.test(utf8Content);
        const hasBrokenChars = /閿燂拷/.test(utf8Content);
        
        // 婵″倹鐏夐崠鍛�鎯堟稉锟介弬鍥х摟缁楋缚绗栭崙铏瑰箛娑旇京鐖滈敍灞界毦鐠囨洑绮燝B2312鏉烇拷閹癸拷
        if (hasChineseChar || hasBrokenChars) {
          try {
            // 娴犲定B2312鏉烇拷閹存€砊F-8
            const content = iconv.decode(buffer, 'gb2312');
            
            // 閸愭瑥娲栭弬鍥︽�㈤敍鍦睺F-8缂傛牜鐖滈敍锟�
            fs.writeFileSync(fullPath, content, 'utf8');
            console.log(`瀹歌尪娴嗛幑锟�: ${fullPath}`);
          } catch (e) {
            console.error(`鏉烇拷閹广垹銇戠拹锟�: ${fullPath}`, e.message);
          }
        }
      } catch (err) {
        console.error(`婢跺嫮鎮婇弬鍥︽�㈡径杈�瑙�: ${fullPath}`, err.message);
      }
    }
  }
}

// 瀵�鈧�婵�瀣�锟藉嫮鎮�
console.log('瀵�鈧�婵�瀣�鐨㈤弬鍥︽�㈡禒宥˙2312鏉烇拷閹癸拷娑撶��TF-8...');
processDirectory(rootDir);
console.log('鏉烇拷閹广垹鐣�閹达拷!'); 