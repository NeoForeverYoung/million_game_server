const fs = require('fs');
const iconv = require('iconv-lite');

// �鸌摰𡁏��隞嗥��銝剜��瘜券�𦠜�惩��
const commentMap = {
  '--頩��㧥��誯鰵撊����': '--�鍳�𢆡��滨蔭',
  '--��𡄯蕭�錬��伐蕭瘞祉黢瘨橒蕭摰詻�斤�𠉛遝頝兩䲰': '--霈曄蔭憭𡁜�睲葵撌乩�𦦵瑪蝔�',
  '--�錬��梶���𡵆��㯄𨫢�𡠺皝���婙�����擀璆�畾烐童撊����': '--�鍂C霂剛����嗵����滚𦛚璅∪�㛖��雿滨蔭',
  '--��𥕦�脲�鞟�寞偏蝝𡁻�𡄯蕭��𢛵�扳�𤑳�梹蕭瘨𣏾�祆�橒蕭���撊�憪�': '--餈𤤿�钅�賢鍳�𢆡��𡒊��蝚砌��銝芣�滚𦛚',
  '--bootstrap�鰵撊����璊歹蕭': '--bootstrap�������㺭',
  '--瘨梶�䀹����婙�喳����辷蕭': '--�鍳�𢆡�����滚𦛚',
  '--��𥕦�脲�鞟�寞偏蝝𡁏�枏��憡��錬��文�峕�惩漱憒剝�鞱����撖殷蕭': '--餈𤤿�𧢲糓�炏�鍳�鍂憭朞����寞芋撘�',
  '--lua�鰵撊����璊斤烵蝝䠷����𥟇���摚����𡡅��𨥈蕭': '--lua��滚𦛚�鍳�𢆡�𧒄���摰帋��',
  '--��𡁜滿敶游旬��喟插��𥕦�萘����蓥蝴璊�撖栽�祇�𡄯蕭��𨥈蕭': '--��𤾸蝱璅∪�𧶏�屸��閬��𧒄�虾��滨蔭'
};

const filePath = 'Chapter2/1_config';

try {
  // 霂餃�𡝗��隞嗅��摰�
  const content = fs.readFileSync(filePath, 'utf8');
  
  // �𤜯�揢�����劐葉���銋梁��
  let newContent = content;
  for (const [garbled, chinese] of Object.entries(commentMap)) {
    // �鸌畾𠰴�����甇���躰”颲曉�譍葉����鸌畾𠰴�㛖泵
    const escapedGarbled = garbled.replace(/[-\/\\^$*+?.()|[\]{}]/g, '\\$&');
    newContent = newContent.replace(new RegExp(escapedGarbled, 'g'), chinese);
  }
  
  // ��坔�墧��隞�
  fs.writeFileSync(filePath, newContent, 'utf8');
  console.log(`撌脖耨憭�: ${filePath}`);
} catch (err) {
  console.error(`憭�������隞嗅仃韐�: ${filePath}`, err.message);
}

console.log('頧祆揢摰峕��!'); 