#include <fstream>
#include "Warning.h"
#include "MpmInterpreter.h"
#include <vector>
#include <iostream>
#include "Json/json.hpp"
#include <sstream>

using json = nlohmann::json;

int materialId = 0;
int bodyId = 0;

namespace MpmInterpreter
{
  // Initially gets bodies and materials names
  void interpret(json &jsonFile, std::string key, std::string value, std::vector<std::string> &linesToCheck);

  // Interprets other keys that are not bodies or materials names or attributes of bodies or materials
  void interpretKey(json &jsonFile, std::string key, std::string value, std::vector<std::string> &linesToCheck);

  // For bodies, get points from file
  void getBodyPointsFromFile(json &jsonFile, const std::string &bodyName, const std::string &filename);

  // After reading the file, check the attributes of bodies and materials
  void checkRemainingLines(json &jsonFile, const std::vector<std::string> &linesToCheck);

  // --- Utility functions ---
  void createJsonObjectIfNotExists(json &jsonFile, const std::string &key);
  void removeEquals(std::string &str);
  std::string toLower(const std::string &s);
  std::string removeQuotes(const std::string &str, bool to_lower = true);
  void checkCsvFormat(const std::string &path, const std::string &bodyName, json &jsonFile);
  void checkJsonFormat(const std::string &path, const std::string &bodyName, json &jsonFile);
}

std::string MpmInterpreter::interpreter(const std::string &filename)
{
  json jsonFile;
  std::vector<std::string> linesToCheck;

  // open the file
  std::ifstream file(filename);
  if (!file.is_open())
  {
    Warning::printMessage("Was not possible read the file,\nplease check the input file name...");
    throw std::runtime_error("Was not possible read the file,\nplease check the input file name...");
  }

  jsonFile["source"] = filename;

  // read the file line by line
  std::string line;
  std::string content;
  while (std::getline(file, line))
  {
    if (line.empty() || line[0] == '#' || line[0] == ';')
    {
      continue; // skip empty lines and comments
    }
    // remove spaces between key and value
    line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
    MpmInterpreter::interpret(jsonFile, line.substr(0, line.find('=')), line.substr(line.find('=') + 1), linesToCheck);
    content += line + "\n";
  }
  file.close();

  checkRemainingLines(jsonFile, linesToCheck);

  std::ofstream outputfile("data.json");
  if (outputfile.is_open())
  {
    outputfile << jsonFile.dump(4); // dump(4) = indentación de 4 espacios
    outputfile.close();
  }
  else
  {
    Warning::printMessage("Could not open output file to write JSON data.");
    throw std::runtime_error("Could not open output file to write JSON data.");
  }

  outputfile.close();

  return "data.json";
}

void MpmInterpreter::interpret(json &jsonFile, std::string key, std::string value, std::vector<std::string> &linesToCheck)
{
  try
  {
    if (key == "materials.names")
    {
      std::vector<std::string> materials = static_cast<std::vector<std::string>>(json::parse(value));
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "material");
      for (const std::string &material : materials)
      {
        MpmInterpreter::createJsonObjectIfNotExists(jsonFile["material"], material);
        jsonFile["material"][material]["id"] = materialId++;
      }
      return;
    }

    if (key == "bodies.names")
    {
      std::vector<std::string> bodies = static_cast<std::vector<std::string>>(json::parse(value));
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "body");
      for (const std::string &body : bodies)
      {
        MpmInterpreter::createJsonObjectIfNotExists(jsonFile["body"], body);
        jsonFile["body"][body]["id"] = bodyId++;
      }
      return;
    }

    // Unknown key
    MpmInterpreter::interpretKey(jsonFile, key, value, linesToCheck);
  }
  catch (...)
  {
    Warning::printMessage("Error interpreting material names with value: " + value);
    linesToCheck.push_back(key + "=" + value);
    return;
  }
}

void MpmInterpreter::interpretKey(json &jsonFile, std::string key, std::string value, std::vector<std::string> &linesToCheck)
{
  try
  {
    // Interpret the key
    if (key == "stress_scheme_update")
    {
      jsonFile["stress_scheme_update"] = MpmInterpreter::removeQuotes(value, false);
      return;
    }

    if (key == "shape_function")
    {
      jsonFile["shape_function"] = MpmInterpreter::removeQuotes(value, false);
      return;
    }

    if (key == "time")
    {
      jsonFile["time"] = std::stod(value);
      return;
    }

    if (key == "time_step")
    {
      jsonFile["time_step"] = std::stod(value);
      return;
    }

    if (key == "n_threads")
    {
      jsonFile["n_threads"] = std::stoi(value);
      return;
    }

    if (key == "gravity")
    {
      jsonFile["gravity"] = static_cast<json::array_t>(json::parse(value));
      return;
    }

    if (key == "damping.type")
    {
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "damping");
      jsonFile["damping"]["type"] = removeQuotes(value);
      return;
    }

    if (key == "damping.value")
    {
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "damping");
      jsonFile["damping"]["value"] = std::stod(value);
      return;
    }

    if (key == "results.print")
    {
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "results");
      jsonFile["results"]["print"] = std::stoi(value);
      return;
    }

    if (key == "results.particles.fields")
    {
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "results");
      jsonFile["results"]["material_point_results"] = static_cast<json::array_t>(json::parse(value));
      return;
    }

    if (key == "results.grid.fields")
    {
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "results");
      jsonFile["results"]["grid_nodal_results"] = static_cast<json::array_t>(json::parse(value));
      return;
    }

    if (key == "mesh.cells.dimension")
    {
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "mesh");
      jsonFile["mesh"]["cells_dimension"] = static_cast<json::array_t>(json::parse(value));
      return;
    }

    if (key == "mesh.cells.number")
    {
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "mesh");
      jsonFile["mesh"]["cells_number"] = static_cast<json::array_t>(json::parse(value));
      return;
    }

    if (key == "mesh.origin")
    {
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "mesh");
      jsonFile["mesh"]["origin"] = static_cast<json::array_t>(json::parse(value));
      return;
    }

    if (key == "mesh.boundary.plane_x0")
    {
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "mesh");
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile["mesh"], "boundary_conditions");
      jsonFile["mesh"]["boundary_conditions"]["plane_X0"] = removeQuotes(value);
      return;
    }

    if (key == "mesh.boundary.plane_y0")
    {
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "mesh");
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile["mesh"], "boundary_conditions");
      jsonFile["mesh"]["boundary_conditions"]["plane_Y0"] = removeQuotes(value);
      return;
    }

    if (key == "mesh.boundary.plane_z0")
    {
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "mesh");
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile["mesh"], "boundary_conditions");
      jsonFile["mesh"]["boundary_conditions"]["plane_Z0"] = removeQuotes(value);
      return;
    }

    if (key == "mesh.boundary.plane_xn")
    {
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "mesh");
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile["mesh"], "boundary_conditions");
      jsonFile["mesh"]["boundary_conditions"]["plane_Xn"] = removeQuotes(value);
      return;
    }

    if (key == "mesh.boundary.plane_yn")
    {
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "mesh");
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile["mesh"], "boundary_conditions");
      jsonFile["mesh"]["boundary_conditions"]["plane_Yn"] = removeQuotes(value);
      return;
    }

    if (key == "mesh.boundary.plane_zn")
    {
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile, "mesh");
      MpmInterpreter::createJsonObjectIfNotExists(jsonFile["mesh"], "boundary_conditions");
      jsonFile["mesh"]["boundary_conditions"]["plane_Zn"] = removeQuotes(value);
      return;
    }

    // Unknown key
    linesToCheck.push_back(key + "=" + value);
  }
  catch (...)
  {
    Warning::printMessage("Error interpreting key: " + key + " with value: " + value);
    linesToCheck.push_back(key + "=" + value);
    return;
  }
}

void MpmInterpreter::getBodyPointsFromFile(json &jsonFile, const std::string &bodyName, const std::string &filename)
{
  // determine the path to the file
  std::string source = jsonFile["source"];
  std::string path = filename;
  if (filename.find('/') == std::string::npos && filename.find('\\') == std::string::npos)
  {
    size_t lastSlash = source.find_last_of("/\\");
    if (lastSlash != std::string::npos)
    {
      path = source.substr(0, lastSlash + 1) + filename;
    }
  }

  if (path.find(".csv") != std::string::npos)
  {
    MpmInterpreter::checkCsvFormat(path, bodyName, jsonFile);
    return;
  }

  if (path.find(".json") != std::string::npos)
  {
    MpmInterpreter::checkJsonFormat(path, bodyName, jsonFile);
    return;
  }

  Warning::printMessage("Unsupported file format for body points: " + filename);
}

void MpmInterpreter::checkJsonFormat(const std::string &path, const std::string &bodyName, json &jsonFile)
{
  try
  {
    // open the file
    std::ifstream file_stream(path);
    if (!file_stream.is_open())
    {
      Warning::printMessage("Cannot open particle JSON file: " + path);
      throw std::runtime_error("Cannot open particle JSON file: " + path);
    }

    // read the file content
    json pointsJson;
    file_stream >> pointsJson;
    file_stream.close();
    if (!pointsJson.contains("particles") || !pointsJson["particles"].is_array())
    {
      Warning::printMessage("Invalid format in particle JSON file: " + path);
      throw std::runtime_error("Invalid format in particle JSON file: " + path);
    }
    jsonFile["body"][bodyName]["points"] = pointsJson["particles"];
  }
  catch (...)
  {
    Warning::printMessage("Error reading body points from file: " + path);
    throw std::runtime_error("Error reading body points from file: " + path);
  }
}

void MpmInterpreter::checkCsvFormat(const std::string &path, const std::string &bodyName, json &jsonFile)
{
  try
  {
    std::ifstream file(path);
    if (!file.is_open())
    {
      Warning::printMessage("Cannot open particle CSV file: " + path);
      throw std::runtime_error("Cannot open particle CSV file: " + path);
    }

    std::string line;
    json points = json::array();
    while (std::getline(file, line))
    {
      if (line.empty() || line[0] == '#' || line[0] == ';') continue;

      line.erase(0, line.find_first_not_of(" \t"));
      line.erase(line.find_last_not_of(" \t") + 1);
      std::istringstream ss(line);
      std::string token;
      json point = json::array();
      while (std::getline(ss, token, ','))
      {
        try
        {
          point.push_back(std::stod(token));
        }
        catch (...)
        {
          Warning::printMessage("Invalid number in particle CSV file: " + token);
          throw std::runtime_error("Invalid number in particle CSV file: " + token);
        }
      }
      if (!point.empty()) points.push_back(point);
    }

    jsonFile["body"][bodyName]["points"] = points;
  }
  catch (...)
  {
    Warning::printMessage("Error reading body points from CSV file: " + path);
    throw std::runtime_error("Error reading body points from CSV file: " + path);
  }
}

void MpmInterpreter::checkRemainingLines(json &jsonFile, const std::vector<std::string> &linesToCheck)
{
  std::string key;
  std::string value;

  try
  {
    for (const std::string &line : linesToCheck)
    {
      key = line.substr(0, line.find('='));
      value = line.substr(line.find('=') + 1);

      // if start with body. for bodies
      if (key.rfind("body.", 0) == 0)
      {
        std::string bodyName = MpmInterpreter::toLower(key.substr(5, key.find('.', 5) - 5));
        std::string bodyProperty = MpmInterpreter::toLower(key.substr(key.find('.', 5) + 1));

        if (jsonFile["body"].find(bodyName) == jsonFile["body"].end())
        {
          Warning::printMessage("Body name not defined: " + bodyName);
          throw std::runtime_error("Body name not defined: " + bodyName);
        }

        if (bodyProperty == "type")
        {
          jsonFile["body"][bodyName]["type"] = MpmInterpreter::removeQuotes(value);
          continue;
        }

        if (bodyProperty == "extrude.direction")
        {
          jsonFile["body"][bodyName]["extrude_direction"] = MpmInterpreter::removeQuotes(value);
          continue;
        }

        if (bodyProperty == "extrude.distance")
        {
          jsonFile["body"][bodyName]["extrude_displacement"] = std::stod(value);
          continue;
        }

        if (bodyProperty == "discretization")
        {
          jsonFile["body"][bodyName]["discretization_length"] = std::stod(value);
          continue;
        }

        if (bodyProperty == "material")
        {
          jsonFile["body"][bodyName]["material_id"] = jsonFile["material"][MpmInterpreter::removeQuotes(value)]["id"];
          continue;
        }

        if (bodyProperty == "vertices")
        {
          jsonFile["body"][bodyName]["points"] = MpmInterpreter::removeQuotes(value);
          MpmInterpreter::getBodyPointsFromFile(jsonFile, bodyName, jsonFile["body"][bodyName]["points"]);
          continue;
        }

        if (bodyProperty == "point_p1")
        {
          jsonFile["body"][bodyName]["point_p1"] = static_cast<json::array_t>(json::parse(value));
          continue;
        }

        if (bodyProperty == "point_p2")
        {
          jsonFile["body"][bodyName]["point_p2"] = static_cast<json::array_t>(json::parse(value));
          continue;
        }
      }

      // if start with material. for materials
      if (key.rfind("material.", 0) == 0)
      {
        std::string materialName = MpmInterpreter::toLower(key.substr(9, key.find('.', 9) - 9));
        std::string materialProperty = key.substr(key.find('.', 9) + 1);

        if (jsonFile["material"].find(materialName) == jsonFile["material"].end())
        {
          Warning::printMessage("Material name not defined: " + materialName);
          throw std::runtime_error("Material name not defined: " + materialName);
        }

        if (materialProperty == "type")
        {
          jsonFile["material"][materialName]["type"] = MpmInterpreter::removeQuotes(value);
          continue;
        }

        if (materialProperty == "young")
        {
          jsonFile["material"][materialName]["young"] = std::stod(value);
          continue;
        }

        if (materialProperty == "density")
        {
          jsonFile["material"][materialName]["density"] = std::stod(value);
          continue;
        }

        if (materialProperty == "poisson")
        {
          jsonFile["material"][materialName]["poisson"] = std::stod(value);
          continue;
        }

        if (materialProperty == "friction")
        {
          jsonFile["material"][materialName]["friction"] = std::stod(value);
          continue;
        }

        if (materialProperty == "cohesion")
        {
          jsonFile["material"][materialName]["cohesion"] = std::stod(value);
          continue;
        }

        if (materialProperty == "tensile")
        {
          jsonFile["material"][materialName]["tensile"] = std::stod(value);
          continue;
        }
      }

      Warning::printMessage("Unknown key in remaining lines: " + key + " with value: " + value);
    }
  }
  catch (...)
  {
    Warning::printMessage("Error interpreting remaining line with key: " + key + " and value: " + value);
    return;
  }
}

void MpmInterpreter::createJsonObjectIfNotExists(json &jsonFile, const std::string &key)
{
  if (!jsonFile.contains(key))
  {
    jsonFile[key] = json::object();
  }
}

void MpmInterpreter::removeEquals(std::string &str)
{
  size_t pos = str.find('=');
  if (pos != std::string::npos)
  {
    str = str.substr(pos + 1);
  }
}

std::string MpmInterpreter::toLower(const std::string &s)
{
  std::string lower;
  lower.reserve(s.size());
  std::transform(s.begin(), s.end(), std::back_inserter(lower),
                 [](unsigned char c)
                 { return std::tolower(c); });
  return lower;
}

std::string MpmInterpreter::removeQuotes(const std::string &str, bool to_lower)
{
  if (str.size() >= 2 && str.front() == '\"' && str.back() == '\"')
  {
    return str.substr(1, str.size() - 2);
  }
  return to_lower ? MpmInterpreter::toLower(str) : str;
}
