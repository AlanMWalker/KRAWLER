#include <iostream>	
#include <fstream>
#include <string>

using namespace std;

#define MAX_SHADER_FILE_CHARS 10000

int main(int argc, char* argv[])
{
	string outputFilename;
	string outputFileContents;
	char fragContents[MAX_SHADER_FILE_CHARS];
	char vertContents[MAX_SHADER_FILE_CHARS];
	ofstream output;
	ifstream fragFile, vertFile;
	memset(fragContents, '\0', sizeof(char) * MAX_SHADER_FILE_CHARS);
	memset(vertContents, '\0', sizeof(char) * MAX_SHADER_FILE_CHARS);
	char* fragArg = nullptr, *vertArg = nullptr;

	if (argc < 4)
	{
		cout << "Not enough shader files passed!" << endl;
		goto bailout;
	}

	outputFilename = argv[3];

	if (strstr(argv[1], "_f") != NULL)
	{
		fragArg = argv[1];
		vertArg = argv[2];
	}
	else
	{
		fragArg = argv[2];
		vertArg = argv[1];
	}

	fragFile.open(fragArg, ios::in | ios::ate);
	if (fragFile.fail())
	{
		cout << "Unable to open fragment shader file" << endl;
		goto bailout;
	}

	vertFile.open(vertArg, ios::in | ios::ate);
	if (vertFile.fail())
	{
		cout << "Unable to open vertex shader file" << endl;
		goto bailout;
	}

	{ //frag
		long len = static_cast<long>(fragFile.tellg());
		if (len >= MAX_SHADER_FILE_CHARS)
		{
			cout << "Fragment shader too long for ShaderPacker" << endl;
			goto bailout;
		}
		fragFile.seekg(ios::beg);
		fragFile.read(fragContents, len);
		fragFile.close();
		if (len < MAX_SHADER_FILE_CHARS)
		{
			fragContents[len] = '\0';
		}
	}

	{ //vert
		long len = static_cast<long>(vertFile.tellg());
		if (len >= MAX_SHADER_FILE_CHARS)
		{
			cout << "Vertex shader too long for ShaderPacker" << endl;
			goto bailout;
		}
		vertFile.seekg(ios::beg);
		vertFile.read(vertContents, len);
		vertFile.close();
		if (len < MAX_SHADER_FILE_CHARS)
		{
			vertContents[len] = '\0';
		}
	}

	outputFileContents += vertContents;
	outputFileContents += "\n#DIVIDE\n";
	outputFileContents += fragContents;

	output.open(outputFilename, ios::out);

	output << outputFileContents;

	output.close();
	return 0;

	/* IF ANYTHING FAILS */
bailout:
	fragFile.close();
	vertFile.close();
	system("pause");
	return -1;
}