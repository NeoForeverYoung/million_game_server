const fs = require('fs');
const path = require('path');
const iconv = require('iconv-lite');

// 定义要处理的编码列表，按尝试顺序排列
const encodings = ['gb2312', 'gbk', 'gb18030', 'big5'];

// 排除的目录和文件类型
const excludeDirs = ['.git', 'node_modules', 'build', 'dist'];
const excludeExtensions = ['.png', '.jpg', '.jpeg', '.gif', '.svg', '.ico', '.pdf', '.zip', '.tar', '.gz', '.exe', '.dll', '.so', '.o', '.a'];

// 需要修复的文件扩展名
const targetExtensions = ['.js', '.lua', '.h', '.cpp', '.c', '.txt', '', '.md', '.config', '.json'];

// 要处理的目录
const rootDir = '.';

// 检查文件是否包含乱码的函数
function containsGarbledText(content) {
  // 检查常见的乱码模式
  const garbledPatterns = [
    /閰嶇疆/, /鍔ㄧ殑/, /娑堟伅/, /鏂囦欢/, /閫�/, /涓�鏂�/, /鐨�/, /浣�/, /鍙�/, 
    /锛�/, /瑕�/, /杩�/, /澶�/, /鍒�/, /鍔�/, /鍦�/, /褰�/, /璇�/, /绛�/, /鍚�/
  ];
  
  return garbledPatterns.some(pattern => pattern.test(content));
}

// 递归遍历目录处理文件
function processDirectory(directory) {
  try {
    const items = fs.readdirSync(directory);
    
    for (const item of items) {
      // 跳过排除的目录
      if (excludeDirs.includes(item)) {
        continue;
      }
      
      const fullPath = path.join(directory, item);
      
      try {
        const stat = fs.statSync(fullPath);
        
        if (stat.isDirectory()) {
          // 递归处理子目录
          processDirectory(fullPath);
        } else if (stat.isFile()) {
          // 检查文件扩展名
          const ext = path.extname(fullPath).toLowerCase();
          if (excludeExtensions.includes(ext) || 
              !targetExtensions.includes(ext) && targetExtensions.length > 0) {
            continue;
          }
          
          try {
            // 读取文件内容
            const buffer = fs.readFileSync(fullPath);
            const utf8Content = buffer.toString('utf8');
            
            // 检查文件是否已经是正确的UTF-8
            if (!containsGarbledText(utf8Content)) {
              continue; // 文件没问题，跳过
            }
            
            // 尝试不同的编码
            let converted = false;
            for (const encoding of encodings) {
              try {
                const convertedContent = iconv.decode(buffer, encoding);
                
                // 检查转换后的内容是否还有乱码
                if (!containsGarbledText(convertedContent)) {
                  // 写回文件
                  fs.writeFileSync(fullPath, convertedContent, 'utf8');
                  console.log(`已修复 [${encoding}]: ${fullPath}`);
                  converted = true;
                  break;
                }
              } catch (e) {
                // 尝试下一种编码
                continue;
              }
            }
            
            if (!converted) {
              console.log(`无法自动修复: ${fullPath} (尝试了 ${encodings.join(', ')})`);
            }
          } catch (err) {
            console.error(`处理文件失败: ${fullPath}`, err.message);
          }
        }
      } catch (err) {
        console.error(`无法访问: ${fullPath}`, err.message);
      }
    }
  } catch (err) {
    console.error(`无法读取目录: ${directory}`, err.message);
  }
}

// 开始处理
console.log('开始修复漏掉的文件编码问题...');
processDirectory(rootDir);
console.log('处理完成!'); 