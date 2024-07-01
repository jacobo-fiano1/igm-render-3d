#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/AnimationPath>
#include <osg/MatrixTransform>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/StateSet>
#include <osgViewer/Viewer>

// Función para crear una ruta de animación
osg::ref_ptr<osg::AnimationPath> createTranslationPath() {
    osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath();
    animationPath->setLoopMode(osg::AnimationPath::LOOP);

    const double animationDuration = 4.0; // Duración de la animación en segundos
    const unsigned int numSamples = 40;   // Número de muestras de la animación
    const double radius = 1.0;

    for (unsigned int i = 0; i <= numSamples; ++i) {
        double time = (double)i / (double)numSamples * animationDuration;
        double angle = time * osg::PI * 2.0 / animationDuration;

        // Posición circular
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        double z = radius * sin(angle);

        osg::Vec3 position(x, y, z);
        osg::Quat rotation;
        animationPath->insert(time, osg::AnimationPath::ControlPoint(position, rotation));
    }

    return animationPath;
}

// Función para crear una ruta de animación de rotación
osg::ref_ptr<osg::AnimationPath> createRotationPath() {
    osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath();
    animationPath->setLoopMode(osg::AnimationPath::LOOP);

    const double animationDuration = 20.0; // Duración de la animación en segundos
    const unsigned int numSamples = 40;   // Número de muestras de la animación

    for (unsigned int i = 0; i <= numSamples; ++i) {
        double time = (double)i / (double)numSamples * animationDuration;
        double angle = time * osg::PI * 2.0;

        // Rotación alrededor del eje Z
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
    vertices->push_back(osg::Vec3(-0.5f, -0.5f, -0.5f)); // 0
    vertices->push_back(osg::Vec3(0.5f, -0.5f, -0.5f));  // 1
    vertices->push_back(osg::Vec3(0.5f, 0.5f, -0.5f));   // 2
    vertices->push_back(osg::Vec3(-0.5f, 0.5f, -0.5f));  // 3
    vertices->push_back(osg::Vec3(-0.5f, -0.5f, 0.5f));  // 4
    vertices->push_back(osg::Vec3(0.5f, -0.5f, 0.5f));   // 5
    vertices->push_back(osg::Vec3(0.5f, 0.5f, 0.5f));    // 6
    vertices->push_back(osg::Vec3(-0.5f, 0.5f, 0.5f));   // 7

    // Definir los colores para cada vértice (para crear un gradiente)
    colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Rojo
    colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f)); // Verde
    colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f)); // Azul
    colors->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f)); // Amarillo
    colors->push_back(osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f)); // Magenta
    colors->push_back(osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f)); // Cian
    colors->push_back(osg::Vec4(1.0f, 0.5f, 0.0f, 1.0f)); // Naranja
    colors->push_back(osg::Vec4(0.5f, 0.0f, 0.5f, 1.0f)); // Púrpura

    // Definir los índices para las caras del cubo
    // Cara frontal
    indices->push_back(4); indices->push_back(5); indices->push_back(6); indices->push_back(7);
    // Cara trasera
    indices->push_back(0); indices->push_back(1); indices->push_back(2); indices->push_back(3);
    // Cara izquierda
    indices->push_back(0); indices->push_back(4); indices->push_back(7); indices->push_back(3);
    // Cara derecha
    indices->push_back(1); indices->push_back(5); indices->push_back(6); indices->push_back(2);
    // Cara superior
    indices->push_back(3); indices->push_back(2); indices->push_back(6); indices->push_back(7);
    // Cara inferior
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

    // Crear un nodo transformador para animar el cubo y alejarlo
    osg::ref_ptr<osg::PositionAttitudeTransform> translationNode = new osg::PositionAttitudeTransform();
    translationNode->setPosition(osg::Vec3(0, -4, 0)); // Alejar el cubo 
    translationNode->addChild(rotationNode);  // Añadir el nodo de rotación al nodo de traslación

    // Crear la ruta de animación de traslación y agregarla al nodo de transformación
    osg::ref_ptr<osg::AnimationPathCallback> translationCallback = new osg::AnimationPathCallback();
    translationCallback->setAnimationPath(createTranslationPath());
    translationNode->setUpdateCallback(translationCallback);

    // Crear un grupo para el cubo y añadir el nodo de transformación a él
    osg::ref_ptr<osg::Group> cubeGroup = new osg::Group();
    cubeGroup->addChild(translationNode);


    // Crear la luz
    osg::ref_ptr<osg::Light> light = new osg::Light();
    light->setLightNum(0);
    light->setPosition(osg::Vec4(0.0f, 0.0f, 10.0f, 1.0f));
    light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
    light->setSpecular(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
    light->setAmbient(osg::Vec4(0.2f, 0.2f, 0.2f, 1.0f));

    // Crear una fuente de luz y asociarla a la luz
    osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
    lightSource->setLight(light);

    // Crear un grupo para la luz y añadir la fuente de luz a él
    osg::ref_ptr<osg::Group> lightGroup = new osg::Group();
    lightGroup->addChild(lightSource);

    // Obtener el StateSet del grupo de luz y habilitar la iluminación
    osg::ref_ptr<osg::StateSet> lightSS = lightGroup->getOrCreateStateSet();
    lightSS->setMode(GL_LIGHTING, osg::StateAttribute::ON);


// Crear un nodo raíz y agregar ambos grupos a él
    osg::ref_ptr<osg::Group> root = new osg::Group();
    root->addChild(cubeGroup);
    root->addChild(lightGroup);


    // Crear un visor y establecer la escena
    osgViewer::Viewer viewer;
    viewer.setSceneData(root);

    // Iniciar el bucle de visualización
    return viewer.run();
}
