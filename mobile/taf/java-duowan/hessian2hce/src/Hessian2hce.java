

import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public final class Hessian2hce
{
	private static Map<Class<?>, String> supportTypes = new HashMap<Class<?>, String>();
	
	private static String[] ignorePackages = {"java.", "javax."}; 
	
	private Map<Class<?>, Class<?>> generateType = new HashMap<Class<?>, Class<?>>();
	
	static
	{
		supportTypes.put(int.class, "int");
		supportTypes.put(short.class, "short");
		supportTypes.put(long.class, "long");
		supportTypes.put(double.class, "double");
		supportTypes.put(float.class, "float");
		supportTypes.put(char.class, "string");
		supportTypes.put(byte.class, "byte");
		supportTypes.put(boolean.class, "bool");
		supportTypes.put(Integer.class, "int");
		supportTypes.put(Short.class, "short");
		supportTypes.put(Long.class, "long");
		supportTypes.put(String.class, "string");
		supportTypes.put(Double.class, "double");
		supportTypes.put(Float.class, "float");
		supportTypes.put(Character.class, "string");
		supportTypes.put(Byte.class, "byte");
		supportTypes.put(Boolean.class, "bool");
		supportTypes.put(Integer[].class, "vector<int>");
		supportTypes.put(Short[].class, "vector<short>");
		supportTypes.put(Long[].class, "vector<long>");
		supportTypes.put(String[].class, "vector<string>");
		supportTypes.put(Double[].class, "vector<double>");
		supportTypes.put(Float[].class, "vector<float>");
		supportTypes.put(Character[].class, "string");
		supportTypes.put(Byte[].class, "binary");
		supportTypes.put(Boolean[].class, "vector<bool>");
		supportTypes.put(int[].class, "vector<int>");
		supportTypes.put(short[].class, "vector<short>");
		supportTypes.put(long[].class, "vector<long>");
		supportTypes.put(double[].class, "vector<double>");
		supportTypes.put(float[].class, "vector<float>");
		supportTypes.put(char[].class, "string");
		supportTypes.put(byte[].class, "binary");
		supportTypes.put(boolean[].class, "vector<bool>");
	}
	
	private ArrayList<Class<?>> allInterface = new ArrayList<Class<?>>();
	private URLClassLoader ucl;
	
	public Hessian2hce(String jarFile) throws IOException, ClassNotFoundException
	{
		String[] s = {jarFile};
		g(s);
	}
	
	public Hessian2hce(String[] jarFiles) throws IOException, ClassNotFoundException
	{
		g(jarFiles);
	}
	
	private void g(String[] jarFiles) throws IOException, ClassNotFoundException
	{
		URL[] urls = new URL[jarFiles.length];
		for(int i = 0; i < urls.length; ++i)
			urls[i] = new URL("file:///" + new File(jarFiles[i]).getAbsolutePath());
		
		ArrayList<String> l = getAllClasses(jarFiles[0]);
		ucl = URLClassLoader.newInstance(urls);
		for (int i = 0; i < l.size(); ++i) {
			Class<?> c = ucl.loadClass(l.get(i));
			if (!c.isInterface())
				continue;
			allInterface.add(c);
		}
	}
	
	public void generateAll(String moduleName, PrintStream out)
	{
		out.print("module ");
		out.println(moduleName);
		out.println("{");
		for(int i = 0; i < allInterface.size(); ++i)
			generate(allInterface.get(i), out);
		out.println("};");
	}
	
	private void generate(Class<?> hessianInterface, PrintStream out)
	{
		if(!hessianInterface.isInterface())
			throw new RuntimeException("not an interface.");
		//out.println("module ?");
		//out.println("{");
		Method[] ms = hessianInterface.getMethods();
		boolean[] isIgnore = new boolean[ms.length];
		// 生成类型定义
		for(int i = 0; i < ms.length; ++i){
			Method m = ms[i];
			Class<?> returnType = m.getReturnType();
			if(!returnType.equals(void.class))
				generateType(returnType, out);
			Class<?>[] parameterTypes = m.getParameterTypes();
			for(int j = 0; j < parameterTypes.length; ++j){
				boolean succ = generateType(parameterTypes[j], out);
				if(!succ){
					isIgnore[i] = true;
					break;
				}
			}
		}
		// 生成函数定义
		boolean isEmpty = true;
		for(int i = 0; i < isIgnore.length; ++i){
			if(!isIgnore[i]){
				isEmpty = false;
				break;
			}
		}
		if(isEmpty)
			out.println("/*");
		out.print("\tinterface ");
		out.println(hessianInterface.getSimpleName());
		out.println("\t{");
		for(int i = 0; i < ms.length; ++i){
			out.print("\t\t");
			if(isIgnore[i])
				out.print("//");
			out.print(getTypeName(ms[i].getReturnType()));
			out.print(" ");
			out.print(ms[i].getName());
			out.print("(");
			Class<?>[] parameterTypes = ms[i].getParameterTypes();
			for(int j = 0; j < parameterTypes.length; ++j){
				if(j > 0)
					out.print(",");
				out.print(getTypeName(parameterTypes[j]));
				out.print(" a" + j);
			}
			out.println(");");
		}
		out.println("\t};");
		if(isEmpty)
			out.println("*/");
		out.println();
	}
	
	private static boolean hasInterface(Class<?> customType, Class<?> class1)
	{
		if(customType.equals(class1))
			return true;
		Class<?>[] cs = customType.getInterfaces();
		for(int i = 0; i < cs.length; ++i){
			if(hasInterface(cs[i], class1))
				return true;
		}
		return false;
	}
	
	private static String getTypeName(Class<?> cls)
	{
		if (cls.isArray()) {
			return "vector<" + getTypeName(cls.getComponentType()) + ">";
		} else if (hasInterface(cls, Map.class)) {
			return "map<?, ?>";
		} else if (hasInterface(cls, List.class)) {
			return "vector<?>";
		} 
		String name = supportTypes.get(cls);
		if (name != null)
			return name;
		return cls.getSimpleName();
	}
	
	private boolean generateType(Class<?> customType, PrintStream out)
	{
		while (customType.isArray())
			customType = customType.getComponentType();
		if (isSupportType(customType))
			return true;
		if (hasInterface(customType, Map.class)
				|| hasInterface(customType, List.class))
			return true;
		for (int i = 0; i < ignorePackages.length; ++i) {
			if (customType.getName().startsWith(ignorePackages[i]))
				return false;
		}
		generateType.put(customType, customType);

		// TODO 处理继承情况
		// generateType(customType.getSuperclass(), out);
		// 先生成子类型的定义
		Field[] fields = customType.getDeclaredFields();
		for (int i = 0; i < fields.length; ++i) {
			generateType(fields[i].getType(), out);
		}
		// 生成自身定义
		out.print("\tstruct ");
		out.println(customType.getSimpleName());
		out.println("\t{");
		for (int i = 0; i < fields.length; ++i) {
			if ((fields[i].getModifiers() & java.lang.reflect.Modifier.STATIC) != 0)
				continue;
			out.print("\t\t");
			out.print(getTypeName(fields[i].getType()));
			out.print("\t");
			out.print(fields[i].getName());
			out.println(";");
		}
		out.println("\t};");
		out.println();
		return true;
	}
	
	private boolean isSupportType(Class<?> t)
	{
		return supportTypes.containsKey(t) || generateType.containsKey(t);
	}
	
	private static ArrayList<String> getAllClasses(String jarFile) throws IOException
    {
        String[] s = {jarFile};
        return getAllClasses(s);
    }

    private static ArrayList<String> getAllClasses(String[] jarFiles) throws IOException
    {
        ArrayList<String> l = new ArrayList<String>();
        for(int i = 0; i < jarFiles.length; ++i){
	        ZipFile zf = new ZipFile(jarFiles[i]);
	        Enumeration<? extends ZipEntry> en = zf.entries();
	        while(en.hasMoreElements()){
	            ZipEntry ze = en.nextElement();
	            if(ze.isDirectory())
	                continue;
	            String s = ze.getName();
	            if(s.endsWith(".class") && s.indexOf("$") == -1)
	                l.add(s.substring(0, s.length() - 6).replace('/', '.'));
	        }
        }
        return l;
    }
	
	/**
	 * @param args
	 * @throws IOException 
	 * @throws ClassNotFoundException 
	 */
	public static void main(String[] args) throws IOException, ClassNotFoundException
	{
		String moduleName = "";
		String[] jarFiles;
		if(args.length >= 2){
			moduleName = args[0];
			jarFiles = new String[args.length - 1];
			for(int i = 0; i < jarFiles.length; ++i)
				jarFiles[i] = args[i + 1];
		}else if(args.length == 1){
			jarFiles = args;
			String[] s = jarFiles[0].split("[/\\\\]");
			s = s[s.length - 1].split("\\.");
			s = s[0].split("-");
			moduleName = s[0];
		}else{
			System.out.println("usage: ");
			System.out.println("\tHessian2hce [modulesName] mainJarFile [jarFile1 ... jarFileN]");
			return;
		}
		
		Hessian2hce hj = new Hessian2hce(jarFiles);
		hj.generateAll(moduleName, System.out);
	}

}
