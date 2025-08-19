<template>
  <div class="home-page">
    <!-- Hero Section -->
    <section class="hero">
      <div class="hero-content">
        <div class="hero-animation">
          <canvas ref="heroCanvas" class="hero-canvas"></canvas>
        </div>
        
        <div class="hero-text">
          <h1 class="hero-title">
            <span class="gradient-text">Context7</span>
            <span class="subtitle">AI-Powered VR Analysis System</span>
          </h1>
          
          <p class="hero-description">
            Revolutionary AI-driven analysis for VR compatibility across 2000+ games. 
            Context7 learns from every analysis, providing increasingly accurate 
            predictions and optimization recommendations.
          </p>
          
          <div class="hero-stats">
            <div class="stat-box">
              <span class="stat-number">{{ stats.totalGames.toLocaleString() }}</span>
              <span class="stat-label">Games Analyzed</span>
            </div>
            <div class="stat-box">
              <span class="stat-number">{{ stats.enginesCovered }}</span>
              <span class="stat-label">Engines Supported</span>
            </div>
            <div class="stat-box">
              <span class="stat-number">{{ (stats.averageAccuracy * 100).toFixed(1) }}%</span>
              <span class="stat-label">AI Accuracy</span>
            </div>
          </div>
          
          <div class="hero-actions">
            <router-link to="/analysis" class="btn btn-primary">
              <i class="fas fa-chart-line"></i>
              Start VR Analysis
            </router-link>
            <router-link to="/database" class="btn btn-secondary">
              <i class="fas fa-database"></i>
              Browse Games Database
            </router-link>
          </div>
        </div>
      </div>
    </section>

    <!-- Features Section -->
    <section class="features">
      <div class="container">
        <h2 class="section-title">AI-Powered VR Analysis</h2>
        <div class="features-grid">
          <div class="feature-card">
            <div class="feature-icon">
              <i class="fas fa-brain"></i>
            </div>
            <h3>Neural Network Analysis</h3>
            <p>Deep learning models analyze game engines, performance characteristics, and VR compatibility factors to provide accurate predictions.</p>
          </div>
          
          <div class="feature-card">
            <div class="feature-icon">
              <i class="fas fa-gamepad"></i>
            </div>
            <h3>Cross-Engine Support</h3>
            <p>Supports 20+ game engines including RE Engine, REDengine 4, MT Framework, Unreal Engine, Unity, and more.</p>
          </div>
          
          <div class="feature-card">
            <div class="feature-icon">
              <i class="fas fa-tachometer-alt"></i>
            </div>
            <h3>Performance Prediction</h3>
            <p>Predicts VR performance metrics including FPS, GPU usage, memory requirements, and comfort ratings.</p>
          </div>
          
          <div class="feature-card">
            <div class="feature-icon">
              <i class="fas fa-cogs"></i>
            </div>
            <h3>Optimization Recommendations</h3>
            <p>AI-generated optimization strategies tailored to specific games and hardware configurations.</p>
          </div>
          
          <div class="feature-card">
            <div class="feature-icon">
              <i class="fas fa-chart-area"></i>
            </div>
            <h3>Learning System</h3>
            <p>Continuously learns from user feedback and performance data to improve accuracy over time.</p>
          </div>
          
          <div class="feature-card">
            <div class="feature-icon">
              <i class="fas fa-heart"></i>
            </div>
            <h3>Comfort Analysis</h3>
            <p>Analyzes motion sickness factors and provides comfort recommendations for different user profiles.</p>
          </div>
        </div>
      </div>
    </section>

    <!-- Priority Games Section -->
    <section class="priority-games">
      <div class="container">
        <h2 class="section-title">Featured VR Implementations</h2>
        <div class="games-showcase">
          <div class="game-card implemented" v-for="game in priorityGames" :key="game.id">
            <div class="game-image">
              <img :src="game.image" :alt="game.name" loading="lazy">
              <div class="game-overlay">
                <div class="compatibility-score">
                  <span class="score">{{ (game.vrScore * 100).toFixed(0) }}%</span>
                  <span class="label">VR Compatible</span>
                </div>
              </div>
            </div>
            
            <div class="game-info">
              <h3>{{ game.name }}</h3>
              <p class="game-engine">{{ game.engine }}</p>
              <div class="game-status" :class="game.status">
                <i :class="getStatusIcon(game.status)"></i>
                {{ getStatusText(game.status) }}
              </div>
              
              <div class="game-features">
                <span v-if="game.raytracing" class="feature-tag">
                  <i class="fas fa-sun"></i> Ray Tracing
                </span>
                <span v-if="game.openWorld" class="feature-tag">
                  <i class="fas fa-globe"></i> Open World
                </span>
                <span class="feature-tag">
                  <i class="fas fa-eye"></i> {{ game.perspective }}
                </span>
              </div>
            </div>
          </div>
        </div>
      </div>
    </section>

    <!-- Technology Section -->
    <section class="technology">
      <div class="container">
        <h2 class="section-title">Advanced AI Architecture</h2>
        <div class="tech-grid">
          <div class="tech-item">
            <h3>Compatibility Classifier</h3>
            <p>Multi-layer neural network that analyzes game characteristics and predicts VR compatibility scores with high accuracy.</p>
            <div class="tech-specs">
              <span>4-layer deep network</span>
              <span>Xavier initialization</span>
              <span>Sigmoid activation</span>
            </div>
          </div>
          
          <div class="tech-item">
            <h3>Performance Predictor</h3>
            <p>Regression model that predicts VR performance metrics including FPS, frame time, and resource usage.</p>
            <div class="tech-specs">
              <span>Leaky ReLU activation</span>
              <span>Performance scaling</span>
              <span>Hardware adaptation</span>
            </div>
          </div>
          
          <div class="tech-item">
            <h3>Optimization Engine</h3>
            <p>Attention-based model that generates contextual optimization recommendations for different game types.</p>
            <div class="tech-specs">
              <span>Attention mechanism</span>
              <span>Category scoring</span>
              <span>Context vectors</span>
            </div>
          </div>
          
          <div class="tech-item">
            <h3>Comfort Analyzer</h3>
            <p>Specialized network for analyzing motion sickness factors and comfort ratings for VR experiences.</p>
            <div class="tech-specs">
              <span>Tanh activation</span>
              <span>Motion features</span>
              <span>Comfort optimization</span>
            </div>
          </div>
        </div>
      </div>
    </section>

    <!-- CTA Section -->
    <section class="cta">
      <div class="container">
        <div class="cta-content">
          <h2>Ready to Experience the Future of VR Analysis?</h2>
          <p>Join the revolution in AI-powered VR compatibility analysis. Get started with Context7 today.</p>
          <div class="cta-actions">
            <router-link to="/analysis" class="btn btn-primary btn-large">
              <i class="fas fa-rocket"></i>
              Start Analysis Now
            </router-link>
            <a href="https://github.com/yourusername/uevrLCX" target="_blank" class="btn btn-outline btn-large">
              <i class="fab fa-github"></i>
              View on GitHub
            </a>
          </div>
        </div>
      </div>
    </section>
  </div>
</template>

<script>
import { ref, onMounted, onUnmounted } from 'vue'

export default {
  name: 'HomePage',
  setup() {
    const heroCanvas = ref(null)
    const stats = ref({
      totalGames: 2147,
      enginesCovered: 34,
      averageAccuracy: 0.887
    })
    
    const priorityGames = ref([
      {
        id: 'cyberpunk2077',
        name: 'Cyberpunk 2077',
        engine: 'REDengine 4',
        vrScore: 0.78,
        status: 'implemented',
        raytracing: true,
        openWorld: true,
        perspective: 'First Person',
        image: '/uevrLCX/images/cyberpunk2077.jpg'
      },
      {
        id: 're7',
        name: 'Resident Evil 7',
        engine: 'RE Engine',
        vrScore: 0.92,
        status: 'implemented',
        raytracing: false,
        openWorld: false,
        perspective: 'First Person',
        image: '/uevrLCX/images/re7.jpg'
      },
      {
        id: 'mhw',
        name: 'Monster Hunter World',
        engine: 'MT Framework',
        vrScore: 0.95,
        status: 'implemented',
        raytracing: false,
        openWorld: true,
        perspective: 'Third Person',
        image: '/uevrLCX/images/mhw.jpg'
      }
    ])
    
    let animationFrame = null
    let particles = []
    
    const initHeroAnimation = () => {
      const canvas = heroCanvas.value
      if (!canvas) return
      
      const ctx = canvas.getContext('2d')
      canvas.width = canvas.offsetWidth * window.devicePixelRatio
      canvas.height = canvas.offsetHeight * window.devicePixelRatio
      ctx.scale(window.devicePixelRatio, window.devicePixelRatio)
      
      // Initialize particles
      particles = []
      for (let i = 0; i < 50; i++) {
        particles.push({
          x: Math.random() * canvas.offsetWidth,
          y: Math.random() * canvas.offsetHeight,
          vx: (Math.random() - 0.5) * 0.5,
          vy: (Math.random() - 0.5) * 0.5,
          size: Math.random() * 2 + 1,
          opacity: Math.random() * 0.5 + 0.2
        })
      }
      
      const animate = () => {
        ctx.clearRect(0, 0, canvas.offsetWidth, canvas.offsetHeight)
        
        // Update and draw particles
        particles.forEach(particle => {
          particle.x += particle.vx
          particle.y += particle.vy
          
          // Wrap around edges
          if (particle.x < 0) particle.x = canvas.offsetWidth
          if (particle.x > canvas.offsetWidth) particle.x = 0
          if (particle.y < 0) particle.y = canvas.offsetHeight
          if (particle.y > canvas.offsetHeight) particle.y = 0
          
          // Draw particle
          ctx.beginPath()
          ctx.arc(particle.x, particle.y, particle.size, 0, Math.PI * 2)
          ctx.fillStyle = `rgba(0, 255, 136, ${particle.opacity})`
          ctx.fill()
        })
        
        // Draw connections
        particles.forEach((particle, i) => {
          particles.slice(i + 1).forEach(otherParticle => {
            const dx = particle.x - otherParticle.x
            const dy = particle.y - otherParticle.y
            const distance = Math.sqrt(dx * dx + dy * dy)
            
            if (distance < 100) {
              const opacity = (100 - distance) / 100 * 0.2
              ctx.beginPath()
              ctx.moveTo(particle.x, particle.y)
              ctx.lineTo(otherParticle.x, otherParticle.y)
              ctx.strokeStyle = `rgba(0, 255, 136, ${opacity})`
              ctx.lineWidth = 0.5
              ctx.stroke()
            }
          })
        })
        
        animationFrame = requestAnimationFrame(animate)
      }
      
      animate()
    }
    
    const getStatusIcon = (status) => {
      const icons = {
        'implemented': 'fas fa-check-circle',
        'ready': 'fas fa-clock',
        'in_progress': 'fas fa-spinner',
        'planned': 'fas fa-calendar'
      }
      return icons[status] || 'fas fa-question-circle'
    }
    
    const getStatusText = (status) => {
      const texts = {
        'implemented': 'Implemented',
        'ready': 'Ready',
        'in_progress': 'In Progress',
        'planned': 'Planned'
      }
      return texts[status] || 'Unknown'
    }
    
    onMounted(() => {
      initHeroAnimation()
      
      // Handle window resize
      const handleResize = () => {
        if (heroCanvas.value) {
          initHeroAnimation()
        }
      }
      window.addEventListener('resize', handleResize)
    })
    
    onUnmounted(() => {
      if (animationFrame) {
        cancelAnimationFrame(animationFrame)
      }
    })
    
    return {
      heroCanvas,
      stats,
      priorityGames,
      getStatusIcon,
      getStatusText
    }
  }
}
</script>

<style scoped>
.home-page {
  min-height: 100vh;
}

/* Hero Section */
.hero {
  position: relative;
  min-height: 80vh;
  display: flex;
  align-items: center;
  background: linear-gradient(135deg, #000 0%, #1a1a1a 50%, #000 100%);
  overflow: hidden;
}

.hero-content {
  max-width: 1200px;
  margin: 0 auto;
  padding: 0 2rem;
  position: relative;
  z-index: 2;
}

.hero-animation {
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  z-index: 1;
}

.hero-canvas {
  width: 100%;
  height: 100%;
}

.hero-text {
  position: relative;
  z-index: 2;
}

.hero-title {
  font-size: 4rem;
  font-weight: 700;
  margin-bottom: 1rem;
  line-height: 1.1;
}

.gradient-text {
  background: linear-gradient(135deg, #00ff88 0%, #00cc6a  100%);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
  display: block;
}

.subtitle {
  display: block;
  font-size: 1.5rem;
  color: #ccc;
  font-weight: 300;
  margin-top: 0.5rem;
}

.hero-description {
  font-size: 1.2rem;
  color: #ccc;
  line-height: 1.6;
  margin: 2rem 0;
  max-width: 600px;
}

.hero-stats {
  display: flex;
  gap: 2rem;
  margin: 3rem 0;
}

.stat-box {
  text-align: center;
  padding: 1.5rem;
  background: rgba(0, 255, 136, 0.1);
  border-radius: 1rem;
  border: 1px solid rgba(0, 255, 136, 0.3);
}

.stat-number {
  display: block;
  font-size: 2rem;
  font-weight: bold;
  color: #00ff88;
}

.stat-label {
  display: block;
  font-size: 0.9rem;
  color: #ccc;
  margin-top: 0.5rem;
}

.hero-actions {
  display: flex;
  gap: 1rem;
  margin-top: 3rem;
}

/* Button Styles */
.btn {
  display: inline-flex;
  align-items: center;
  gap: 0.5rem;
  padding: 1rem 2rem;
  border-radius: 0.5rem;
  text-decoration: none;
  font-weight: 600;
  transition: all 0.3s ease;
  border: none;
  cursor: pointer;
}

.btn-primary {
  background: linear-gradient(135deg, #00ff88 0%, #00cc6a 100%);
  color: #000;
}

.btn-primary:hover {
  transform: translateY(-2px);
  box-shadow: 0 10px 20px rgba(0, 255, 136, 0.3);
}

.btn-secondary {
  background: transparent;
  color: #00ff88;
  border: 2px solid #00ff88;
}

.btn-secondary:hover {
  background: #00ff88;
  color: #000;
}

.btn-outline {
  background: transparent;
  color: #ccc;
  border: 2px solid #444;
}

.btn-outline:hover {
  border-color: #00ff88;
  color: #00ff88;
}

.btn-large {
  padding: 1.25rem 2.5rem;
  font-size: 1.1rem;
}

/* Container */
.container {
  max-width: 1200px;
  margin: 0 auto;
  padding: 0 2rem;
}

/* Section Styles */
section {
  padding: 4rem 0;
}

.section-title {
  font-size: 2.5rem;
  text-align: center;
  color: #fff;
  margin-bottom: 3rem;
}

/* Features Section */
.features {
  background: #111;
}

.features-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: 2rem;
}

.feature-card {
  background: #222;
  padding: 2rem;
  border-radius: 1rem;
  border: 1px solid #333;
  transition: all 0.3s ease;
}

.feature-card:hover {
  transform: translateY(-5px);
  border-color: #00ff88;
  box-shadow: 0 10px 30px rgba(0, 255, 136, 0.2);
}

.feature-icon {
  font-size: 3rem;
  color: #00ff88;
  margin-bottom: 1rem;
}

.feature-card h3 {
  color: #fff;
  margin-bottom: 1rem;
  font-size: 1.3rem;
}

.feature-card p {
  color: #ccc;
  line-height: 1.6;
}

/* Priority Games Section */
.priority-games {
  background: #0a0a0a;
}

.games-showcase {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(350px, 1fr));
  gap: 2rem;
}

.game-card {
  background: #222;
  border-radius: 1rem;
  overflow: hidden;
  border: 1px solid #333;
  transition: all 0.3s ease;
}

.game-card:hover {
  transform: translateY(-5px);
  border-color: #00ff88;
  box-shadow: 0 15px 30px rgba(0, 0, 0, 0.5);
}

.game-image {
  position: relative;
  height: 200px;
  background: #333;
  overflow: hidden;
}

.game-image img {
  width: 100%;
  height: 100%;
  object-fit: cover;
}

.game-overlay {
  position: absolute;
  top: 1rem;
  right: 1rem;
}

.compatibility-score {
  background: rgba(0, 0, 0, 0.8);
  padding: 0.5rem 1rem;
  border-radius: 0.5rem;
  text-align: center;
}

.compatibility-score .score {
  display: block;
  font-size: 1.2rem;
  font-weight: bold;
  color: #00ff88;
}

.compatibility-score .label {
  display: block;
  font-size: 0.8rem;
  color: #ccc;
}

.game-info {
  padding: 1.5rem;
}

.game-info h3 {
  color: #fff;
  margin-bottom: 0.5rem;
  font-size: 1.2rem;
}

.game-engine {
  color: #888;
  margin-bottom: 1rem;
  font-size: 0.9rem;
}

.game-status {
  display: inline-flex;
  align-items: center;
  gap: 0.5rem;
  padding: 0.25rem 0.75rem;
  border-radius: 1rem;
  font-size: 0.8rem;
  font-weight: 600;
  margin-bottom: 1rem;
}

.game-status.implemented {
  background: rgba(0, 255, 136, 0.2);
  color: #00ff88;
}

.game-features {
  display: flex;
  flex-wrap: wrap;
  gap: 0.5rem;
}

.feature-tag {
  display: inline-flex;
  align-items: center;
  gap: 0.25rem;
  padding: 0.25rem 0.5rem;
  background: #333;
  border-radius: 0.25rem;
  font-size: 0.8rem;
  color: #ccc;
}

/* Technology Section */
.technology {
  background: #111;
}

.tech-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 2rem;
}

.tech-item {
  background: #222;
  padding: 2rem;
  border-radius: 1rem;
  border-left: 4px solid #00ff88;
}

.tech-item h3 {
  color: #00ff88;
  margin-bottom: 1rem;
  font-size: 1.2rem;
}

.tech-item p {
  color: #ccc;
  line-height: 1.6;
  margin-bottom: 1rem;
}

.tech-specs {
  display: flex;
  flex-wrap: wrap;
  gap: 0.5rem;
}

.tech-specs span {
  padding: 0.25rem 0.5rem;
  background: #333;
  border-radius: 0.25rem;
  font-size: 0.8rem;
  color: #ccc;
}

/* CTA Section */
.cta {
  background: linear-gradient(135deg, #00ff88 0%, #00cc6a 100%);
  color: #000;
}

.cta-content {
  text-align: center;
}

.cta h2 {
  font-size: 2.5rem;
  margin-bottom: 1rem;
  color: #000;
}

.cta p {
  font-size: 1.2rem;
  margin-bottom: 2rem;
  opacity: 0.8;
}

.cta-actions {
  display: flex;
  justify-content: center;
  gap: 1rem;
}

.cta .btn-primary {
  background: #000;
  color: #00ff88;
}

.cta .btn-outline {
  border-color: #000;
  color: #000;
}

.cta .btn-outline:hover {
  background: #000;
  color: #00ff88;
}

/* Responsive Design */
@media (max-width: 768px) {
  .hero-title {
    font-size: 2.5rem;
  }
  
  .hero-stats {
    flex-direction: column;
    gap: 1rem;
  }
  
  .hero-actions {
    flex-direction: column;
  }
  
  .games-showcase,
  .features-grid,
  .tech-grid {
    grid-template-columns: 1fr;
  }
  
  .cta-actions {
    flex-direction: column;
    align-items: center;
  }
}
</style>