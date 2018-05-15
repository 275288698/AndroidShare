package hot;


import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
//最终表达式： 
//我们用@符号将邮箱的名称和域名拼接起来，因此完整的邮箱表达式为 
//^[A-Za-z0-9\u4e00-\u9fa5]+@[a-zA-Z0-9_-]+(\.[a-zA-Z0-9_-]+)+$
//^[\\w[.-]]+               @[\\w[.-]]+    \\.[\\w]+$
//^[\\w[.-]]+@[\\w[.-]]+\\.[\\w]+$
   public static List<String> filter(String html,String regex) {
       List<String> resultList = new ArrayList<String>();
//     Pattern p = Pattern.compile("<head>([\\s\\S]*?)</head>");//正则表达式 commend by danielinbiti
     Pattern p = Pattern.compile(regex);//正则表达式 commend by danielinbiti
       Matcher m = p.matcher(html );//
	    int count=m.groupCount();

	    System.out.println(count+"start =========="+regex);
	    while (m.find()) {
	    	for (int i = 0; i <= count; i++) {
	    		String data=m.group(i); 
//	    		System.out.println(data);
	    		 resultList.add(data);//
			}
//	    	String data=m.group(1);  
//			System.out.println(data);
//	    	 resultList.add(data);//
       }
	    System.out.println("==========end ");
       return resultList;
   }
   
	/**
	 * @param args
	 */
	public static void main(String[] args) {
//		String html = 
//				"<Document> <head><ttsdfsdfdsf><link rel=\"shortcut\" href=\"shortcut.png\"/> <link rel=\"apple-touch-con\" href=\"yiqin.png\"/>  hello</head>" +
//				"<head>world</head></Document>";
//		String regex = "<head>([\\s\\S]*?)<link([\\s\\S]*?)</head>";
//		List<String> headFiter = filter(html, regex);
//		
//		regex = "<link rel=\"app([\\s\\S]*?)/>";
//		
//		List<String> linkFiter = new ArrayList<>();
//		for (int i = 0; i < headFiter.size(); i++) {
//			html = headFiter.get(i);
//			linkFiter.addAll(filter(html, regex));
//		}
//		for (int i = 0; i < linkFiter.size(); i++) {
//			String link = linkFiter.get(i);
//			int index = link.indexOf("href=");
//			if (index==-1) {
//				continue;
//			}
//			int indexS = link.indexOf("\"", index);
//			if (indexS==-1) {
//				continue;
//			}
//			int indexE = link.indexOf("\"", indexS+1);
//			if (index==-1) {
//				continue;
//			}
//			String ret = link.substring(indexS+1,indexE);
//			System.out.println(ret);
//		}
//		String html = "011aaabbbbcccccddddddeeeeeee88888888999999999";
//		String html = "@1@a@	";
//		String regex = "@(\\w)@(\\w)@(\\s)";
//		List<String> headFiter = filter(html, regex);
//		System.out.println(headFiter);
		
		Pattern p2 = Pattern.compile("\\[.++\\]\\[.++\\]");
		Matcher m2 = p2.matcher("[che][1]'s blog is [rebey.cn][2],and built in [2016][3].");
		while(m2.find()) {
		    System.out.println(m2.group());
		}

	}
}