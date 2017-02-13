package com.qq.taf.server.util;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.StringWriter;
import java.util.List;

/**
 * 文件操作工具
 * @author fanzhang
 */
public abstract class FileUtil {

	public static String readFileToString(String fileName,String charset) {
		String result=null;
		StringWriter sw=null;
		BufferedReader br=null;
		try {
			sw=new StringWriter();
			br=new BufferedReader(new InputStreamReader(new FileInputStream(fileName),charset));
			int ch=0;
			while((ch=br.read())!=-1) sw.write(ch); 
			result=sw.toString();
		} catch(Throwable t) {
			GeneralException.raise("FileUtil|read error",t);
		} finally {
			try {
				if(sw!=null) sw.close();
				if(br!=null) br.close();
			} catch(Throwable tt) {
				GeneralException.raise("FileUtil|read error",tt);
			}
		}
		return result;
	}
	
	public static void writeStringToFile(String fileName,String content,String charset) {
		BufferedWriter bw=null;
		try {
			bw=new BufferedWriter(new OutputStreamWriter(new FileOutputStream(fileName),charset));
			bw.write(content);
		} catch(Throwable t) {
			GeneralException.raise("FileUtil|write error",t);
		} finally {
			try {
				bw.close();
			} catch(Throwable tt) {
				GeneralException.raise("FileUtil|write error",tt);
			}
		}
	}
	
	public static void writeLinesToFile(String fileName,List<String> lines,String charset) {
		BufferedWriter bw=null;
		try {
			bw=new BufferedWriter(new OutputStreamWriter(new FileOutputStream(fileName),charset));
			for(String line:lines) bw.write(line+"\n");
			bw.flush();
		} catch(Throwable t) {
			GeneralException.raise("FileUtil|write lines error",t);
		} finally {
			try {
				bw.close();
			} catch(Throwable tt) {
				GeneralException.raise("FileUtil|write lines error",tt);
			}
		}
	}
	
	public static void appendLinesToFile(String fileName,List<String> lines,String charset) {
		BufferedWriter bw=null;
		try {
			bw=new BufferedWriter(new OutputStreamWriter(new FileOutputStream(fileName,true),charset));
			for(String line:lines) bw.write(line.trim()+"\n");
			bw.flush();
		} catch(Throwable t) {
			GeneralException.raise("FileUtil|write lines error",t);
		} finally {
			try {
				bw.close();
			} catch(Throwable tt) {
				GeneralException.raise("FileUtil|write lines error",tt);
			}
		}
	}
	
	public static String readFileToString(String fileName) {
		return readFileToString(fileName,"gbk");
	}
	
	public static void writeStringToFile(String fileName,String content) {
		writeStringToFile(fileName, content,"gbk");
	}
	
	public static void appendLinesToFile(String fileName,List<String> lines) {
		appendLinesToFile(fileName, lines, "gbk");
	}
	
	public static void writeLinesToFile(String fileName,List<String> lines) {
		writeLinesToFile(fileName, lines,"gbk");
	}
	
	public static void main(String[] args) {
		readFileToString("e:/temp/read.txt");
		writeStringToFile("e:/temp/write.txt", "jjjjjjjjjjj");
	}
	
}
