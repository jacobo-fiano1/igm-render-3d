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

osg::ref_ptr<osg::Geode> createColoredCube() {
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_QUADS);

    // Definir los vértices del cubo
    vertices->push_back(osg::Vec3(-0.5f, -0.5f, -0.5f));
    vertices->push_back(osg::Vec3(0.5f, -0.5f, -0.5f));
    vertices->push_back(osg::Vec3(0.5f, 0.5f, -0.5f));
    vertices->push_back(osg::Vec3(-0.5f, 0.5f, -0.5f));
    vertices->push_back(osg::Vec3(-0.5f, -0.5f, 0.5f));
    vertices->push_back(osg::Vec3(0.5f, -0.5f, 0.5f));
    vertices->push_back(osg::Vec3(0.5f, 0.5f, 0.5f));
    vertices->push_back(osg::Vec3(-0.5f, 0.5f, 0.5f));

    // Definir los colores para cada vértice
    colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Rojo
    colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f)); // Verde
    colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f)); // Azul
    colors->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f)); // Amarillo
    colors->push_back(osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f)); // Magenta
    colors->push_back(osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f)); // Cian
    colors->push_back(osg::Vec4(1.0f, 0.5f, 0.0f, 1.0f)); // Naranja
    colors->push_back(osg::Vec4(0.5f, 0.0f, 0.5f, 1.0f)); // Púrpura

    indices->push_back(4); indices->push_back(5); indices->push_back(6); indices->push_back(7);
    indices->push_back(0); indices->push_back(1); indices->push_back(2); indices->push_back(3);
    indices->push_back(0); indices->push_back(4); indices->push_back(7); indices->push_back(3);
    indices->push_back(1); indices->push_back(5); indices->push_back(6); indices->push_back(2);
    indices->push_back(3); indices->push_back(2); indices->push_back(6); indices->push_back(7);
    indices->push_back(0); indices->push_back(1); indices->push_back(5); indices->push_back(4);

    geometry->setVertexArray(vertices);
    geometry->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
    geometry->addPrimitiveSet(indices);

    geode->addDrawable(geometry);
    return geode;
}


int main(int argc, char** argv) {
    // Crear el Geode del cubo coloreado
    osg::ref_ptr<osg::Geode> cuboGeode = createColoredCube();

    // Crear un nodo de transformación para la rotación del cubo
    osg::ref_ptr<osg::PositionAttitudeTransform> rotationNode = new osg::PositionAttitudeTransform();
    rotationNode->addChild(cuboGeode);
    // Crear la ruta de animación de rotación y agregarla al nodo de rotación
    osg::ref_ptr<osg::AnimationPathCallback> rotationCallback = new osg::AnimationPathCallback();
    rotationCallback->setAnimationPath(createRotationPath());
    rotationNode->setUpdateCallback(rotationCallback);
        
    // Crear un nodo raíz y agregar el nodo de rotación a él
    osg::ref_ptr<osg::Group> root = new osg::Group();
    root->addChild(rotationNode);

    // Crear un visor y establecer la escena
    osgViewer::Viewer viewer;
    viewer.setSceneData(root);

    // Configurar el manipulador de cámara para permitir la interacción con el ratón
    osg::ref_ptr<osgGA::TrackballManipulator> manipulator = new osgGA::TrackballManipulator();
    viewer.setCameraManipulator(new osgGA::TrackballManipulator);

    // Iniciar el bucle de visualización
    return viewer.run();
}
