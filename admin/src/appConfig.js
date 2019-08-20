// define network paths to various controllers
const IS_PRODUCTION = false;

const MASTER_PI_IP = '192.168.1.10';
const JAR_1_IP = '192.168.1.11';
const JAR_2_IP = '192.168.1.12';
const JAR_3_IP = '192.168.1.13';

let fireControllerURL, smallFireflyLEDControllerURL, jarLEDControllerURLs, soundControllerURL;

if (IS_PRODUCTION) {
    fireControllerURL = `http://${MASTER_PI_IP}:5000`;
    smallFireflyLEDControllerURL = `http://${MASTER_PI_IP}:7000`;
    jarLEDControllerURLs = [`http://${JAR_1_IP}:8000`, `http://${JAR_2_IP}:8000`, `http://${JAR_3_IP}:8000`];
    soundControllerURL = `http://${MASTER_PI_IP}:9000`;
} else {
    fireControllerURL = `http://localhost:5000`;
    smallFireflyLEDControllerURL = `http://localhost:7000`;
    jarLEDControllerURLs = [`http://localhost:8000`, `http://localhost:8000`, `http://localhost:8000`];
    soundControllerURL = `http://localhost:9000`;
}

export {
    fireControllerURL,
    smallFireflyLEDControllerURL,
    jarLEDControllerURLs,
    soundControllerURL
};
