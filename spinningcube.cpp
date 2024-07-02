#include <iostream>
#include <osgDB/ReadFile>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/AnimationPath>
#include <osg/MatrixTransform>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/StateSet>
#include <osgGA/TrackballManipulator>
#include <osgViewer/Viewer>

// Función para crear una ruta de animación de rotación
osg::ref_ptr<osg::AnimationPath> createRotationPath() {
  osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath();
  animationPath->setLoopMode(osg::AnimationPath::LOOP);

  const double degreesPerSecond = 40.0; // Grados por segundo
  const double sampleInterval = 1.0; // Intervalo de muestreo en segundos
  const unsigned int numSamples = 40;   // Número de muestras de la animación

  for (unsigned int i = 0; i <= numSamples; ++i) {
      double time = i * sampleInterval;
      double angle = osg::DegreesToRadians(degreesPerSecond * time);

      osg::Quat rotation(angle, osg::Vec3(0.0, 1.0, 1.0));
      animationPath->insert(time, osg::AnimationPath::ControlPoint(osg::Vec3(), rotation));
  }

  return animationPath;
}

int main(int argc, char* argv[]) {
  
  // Comprobar parámetros de línea de comandos
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <model file>\n";
    exit(1);
  }

  // Cargar el modelo
  osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(argv[1]);
  if (!loadedModel) {
    std::cerr << "Problem opening '" << argv[1] << "'\n";
    exit(1);
  }

  // Crear el nodo raíz
  osg::ref_ptr<osg::Group> root(new osg::Group());

  // Crear un nodo de transformación para la rotación del cubo
  osg::ref_ptr<osg::PositionAttitudeTransform> rotationNode = new osg::PositionAttitudeTransform();
  rotationNode->addChild(loadedModel);
  // Crear la ruta de animación de rotación y agregarla al nodo de rotación
  osg::ref_ptr<osg::AnimationPathCallback> rotationCallback = new osg::AnimationPathCallback();
  rotationCallback->setAnimationPath(createRotationPath());
  rotationNode->setUpdateCallback(rotationCallback);

  // Crear pat para el primer cubo
  osg::ref_ptr<osg::PositionAttitudeTransform> cube1 = new osg::PositionAttitudeTransform();
  cube1->addChild(rotationNode);
  cube1->setPosition(osg::Vec3(-2.0, 0.0, 0.0));

  // Crear para para el segundo cubo
  osg::ref_ptr<osg::PositionAttitudeTransform> cube2 = new osg::PositionAttitudeTransform();
  cube2->addChild(rotationNode);
  cube2->setPosition(osg::Vec3(2.0, 0.0, 0.0));

  // Añadir nueva fuente de luz
  osg::ref_ptr<osg::PositionAttitudeTransform> lightPAT(new osg::PositionAttitudeTransform());
  lightPAT->setPosition(osg::Vec3(5.0, 12.0, 3.0));
  lightPAT->setScale(osg::Vec3(0.1, 0.1, 0.1));
  root->addChild(lightPAT);

  // Setup GL_LIGHT1. Leave GL_LIGHT0 as it is by default (enabled)
  osg::ref_ptr<osg::LightSource> lightSource(new osg::LightSource());
  lightSource->addChild(loadedModel);
  lightSource->getLight()->setLightNum(1);
  lightSource->getLight()->setPosition(osg::Vec4(0.0, 0.0, 0.0, 1.0));
  lightSource->getLight()->setDiffuse(osg::Vec4(1.0, 1.0, 0.0, 1.0));

  lightPAT->addChild(lightSource);

  osg::ref_ptr<osg::StateSet> ss = root->getOrCreateStateSet();
  ss->setMode(GL_LIGHT1, osg::StateAttribute::ON);
      
  // Agregar los nodos con los cubos al nodo raíz
  root->addChild(cube1);
  root->addChild(cube2);

  // Crear un visor y establecer la escena
  osgViewer::Viewer viewer;
  viewer.setSceneData(root);

  // Configurar el manipulador de cámara para permitir la interacción con el ratón
  osg::ref_ptr<osgGA::TrackballManipulator> manipulator = new osgGA::TrackballManipulator();
  viewer.setCameraManipulator(new osgGA::TrackballManipulator);

  // Iniciar el bucle de visualización
  return viewer.run();
}
