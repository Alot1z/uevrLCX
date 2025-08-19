/**
 * uevrLCX Main JavaScript
 * Provides interactive functionality for the GitHub Pages site
 */

// Global configuration
const CONFIG = {
    apiBaseUrl: 'https://api.github.com/repos/your-repo/uevrLCX',
    reportsPath: '../reports',
    updateInterval: 30000, // 30 seconds
    maxRetries: 3
};

// Global state
let gameData = [];
let performanceData = {};
let currentFilters = {};

// Initialize when DOM is ready
document.addEventListener('DOMContentLoaded', function() {
    initializeSite();
});

/**
 * Initialize the site functionality
 */
function initializeSite() {
    console.log('Initializing uevrLCX site...');
    
    // Initialize components
    initializeGameDatabase();
    initializePerformanceCharts();
    initializeSearchFunctionality();
    initializeRealTimeUpdates();
    
    // Load initial data
    loadGameData();
    loadPerformanceData();
    
    console.log('Site initialization complete');
}

/**
 * Initialize the game compatibility database
 */
function initializeGameDatabase() {
    const searchInput = document.getElementById('game-search');
    const searchBtn = document.getElementById('search-btn');
    const gamesTable = document.getElementById('games-table');
    
    if (searchInput && searchBtn) {
        searchBtn.addEventListener('click', () => performSearch(searchInput.value));
        searchInput.addEventListener('keypress', (e) => {
            if (e.key === 'Enter') {
                performSearch(searchInput.value);
            }
        });
    }
    
    if (gamesTable) {
        // Add sorting functionality
        addTableSorting(gamesTable);
    }
}

/**
 * Initialize performance charts
 */
function initializePerformanceCharts() {
    const perfChart = document.getElementById('perf-chart');
    if (perfChart) {
        createPerformanceChart(perfChart);
    }
}

/**
 * Initialize search functionality
 */
function initializeSearchFunctionality() {
    // Global search across all content
    const searchInputs = document.querySelectorAll('.search-input');
    searchInputs.forEach(input => {
        input.addEventListener('input', debounce(handleGlobalSearch, 300));
    });
}

/**
 * Initialize real-time updates
 */
function initializeRealTimeUpdates() {
    // Set up periodic updates
    setInterval(updateLiveData, CONFIG.updateInterval);
    
    // Initial update
    updateLiveData();
}

/**
 * Load game compatibility data
 */
async function loadGameData() {
    try {
        // Try to load from local reports first
        const response = await fetch(`${CONFIG.reportsPath}/games.json`);
        if (response.ok) {
            gameData = await response.json();
            updateGameTable();
        } else {
            // Fallback to sample data
            gameData = getSampleGameData();
            updateGameTable();
        }
    } catch (error) {
        console.warn('Could not load game data, using sample data:', error);
        gameData = getSampleGameData();
        updateGameTable();
    }
}

/**
 * Load performance data
 */
async function loadPerformanceData() {
    try {
        const response = await fetch(`${CONFIG.reportsPath}/performance.json`);
        if (response.ok) {
            performanceData = await response.json();
            updatePerformanceCharts();
        }
    } catch (error) {
        console.warn('Could not load performance data:', error);
    }
}

/**
 * Update the game compatibility table
 */
function updateGameTable() {
    const tbody = document.getElementById('games-tbody');
    if (!tbody || !gameData.length) return;
    
    let html = '';
    gameData.forEach(game => {
        html += `
            <tr>
                <td><strong>${game.name}</strong></td>
                <td>${game.engine}</td>
                <td><span class="status-${game.vrStatus.toLowerCase()}">${getStatusIcon(game.vrStatus)} ${game.vrStatus}</span></td>
                <td>${game.performance}</td>
                <td>${game.lastTested}</td>
                <td>
                    <a href="../engines/${game.engine.toLowerCase().replace(' ', '-')}.html" class="btn btn-small">View</a>
                    <a href="../downloads/${game.engine.toLowerCase().replace(' ', '-')}-adapter.zip" class="btn btn-small">Download</a>
                </td>
            </tr>
        `;
    });
    
    tbody.innerHTML = html;
}

/**
 * Perform search across games
 */
function performSearch(query) {
    if (!query.trim()) {
        updateGameTable(); // Show all games
        return;
    }
    
    const filteredGames = gameData.filter(game => 
        game.name.toLowerCase().includes(query.toLowerCase()) ||
        game.engine.toLowerCase().includes(query.toLowerCase()) ||
        game.vrStatus.toLowerCase().includes(query.toLowerCase())
    );
    
    updateGameTableWithData(filteredGames);
}

/**
 * Update game table with specific data
 */
function updateGameTableWithData(games) {
    const tbody = document.getElementById('games-tbody');
    if (!tbody) return;
    
    let html = '';
    if (games.length === 0) {
        html = '<tr><td colspan="6" style="text-align: center;">No games found matching your search.</td></tr>';
    } else {
        games.forEach(game => {
            html += `
                <tr>
                    <td><strong>${game.name}</strong></td>
                    <td>${game.engine}</td>
                    <td><span class="status-${game.vrStatus.toLowerCase()}">${getStatusIcon(game.vrStatus)} ${game.vrStatus}</span></td>
                    <td>${game.performance}</td>
                    <td>${game.lastTested}</td>
                    <td>
                        <a href="../engines/${game.engine.toLowerCase().replace(' ', '-')}.html" class="btn btn-small">View</a>
                        <a href="../downloads/${game.engine.toLowerCase().replace(' ', '-')}-adapter.zip" class="btn btn-small">Download</a>
                    </td>
                </tr>
            `;
        });
    }
    
    tbody.innerHTML = html;
}

/**
 * Create performance chart
 */
function createPerformanceChart(canvas) {
    const ctx = canvas.getContext('2d');
    
    // Sample performance data
    const data = {
        labels: ['MT Framework', 'RE Engine', 'REDengine 4', 'Unreal Engine', 'Unity Engine'],
        datasets: [{
            label: 'Average FPS',
            data: [90, 75, 70, 95, 80],
            backgroundColor: [
                'rgba(40, 167, 69, 0.8)',
                'rgba(255, 193, 7, 0.8)',
                'rgba(220, 53, 69, 0.8)',
                'rgba(23, 162, 184, 0.8)',
                'rgba(111, 66, 193, 0.8)'
            ],
            borderColor: [
                'rgba(40, 167, 69, 1)',
                'rgba(255, 193, 7, 1)',
                'rgba(220, 53, 69, 1)',
                'rgba(23, 162, 184, 1)',
                'rgba(111, 66, 193, 1)'
            ],
            borderWidth: 2
        }]
    };
    
    new Chart(ctx, {
        type: 'bar',
        data: data,
        options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: {
                y: {
                    beginAtZero: true,
                    max: 100,
                    title: {
                        display: true,
                        text: 'FPS'
                    }
                },
                x: {
                    title: {
                        display: true,
                        text: 'Engine'
                    }
                }
            },
            plugins: {
                legend: {
                    display: false
                },
                title: {
                    display: true,
                    text: 'Engine Performance Comparison'
                }
            }
        }
    });
}

/**
 * Update performance charts with new data
 */
function updatePerformanceCharts() {
    // This would update existing charts with new data
    // For now, just log the update
    console.log('Performance charts updated');
}

/**
 * Update live data from reports
 */
async function updateLiveData() {
    try {
        // Update recent issues
        await updateRecentIssues();
        
        // Update live test status
        await updateLiveTestStatus();
        
    } catch (error) {
        console.warn('Error updating live data:', error);
    }
}

/**
 * Update recent issues display
 */
async function updateRecentIssues() {
    const issuesContainer = document.getElementById('recent-issues');
    if (!issuesContainer) return;
    
    try {
        const response = await fetch(`${CONFIG.reportsPath}/issues.json`);
        if (response.ok) {
            const issues = await response.json();
            displayRecentIssues(issues, issuesContainer);
        }
    } catch (error) {
        // Use sample data if reports not available
        const sampleIssues = getSampleIssues();
        displayRecentIssues(sampleIssues, issuesContainer);
    }
}

/**
 * Update live test status
 */
async function updateLiveTestStatus() {
    const liveTestsContainer = document.getElementById('live-tests');
    if (!liveTestsContainer) return;
    
    try {
        const response = await fetch(`${CONFIG.reportsPath}/live-tests.json`);
        if (response.ok) {
            const tests = await response.json();
            updateLiveTestDisplay(tests, liveTestsContainer);
        }
    } catch (error) {
        // Live tests will continue to show static data
        console.log('Live test status update skipped');
    }
}

/**
 * Display recent issues
 */
function displayRecentIssues(issues, container) {
    if (!issues || !issues.length) {
        container.innerHTML = '<p>No recent issues reported.</p>';
        return;
    }
    
    let html = '';
    issues.slice(0, 5).forEach(issue => {
        html += `
            <div class="issue-item">
                <span class="issue-severity ${issue.severity.toLowerCase()}">${issue.severity}</span>
                <span class="issue-title">${issue.title}</span>
                <span class="issue-time">${formatTime(issue.timestamp)}</span>
            </div>
        `;
    });
    
    container.innerHTML = html;
}

/**
 * Update live test display
 */
function updateLiveTestDisplay(tests, container) {
    // This would update the live test status indicators
    // For now, just log the update
    console.log('Live test display updated');
}

/**
 * Add table sorting functionality
 */
function addTableSorting(table) {
    const headers = table.querySelectorAll('th');
    headers.forEach((header, index) => {
        header.style.cursor = 'pointer';
        header.addEventListener('click', () => sortTable(table, index));
    });
}

/**
 * Sort table by column
 */
function sortTable(table, columnIndex) {
    const tbody = table.querySelector('tbody');
    const rows = Array.from(tbody.querySelectorAll('tr'));
    
    rows.sort((a, b) => {
        const aValue = a.cells[columnIndex].textContent.trim();
        const bValue = b.cells[columnIndex].textContent.trim();
        
        // Try to parse as number first
        const aNum = parseFloat(aValue);
        const bNum = parseFloat(bValue);
        
        if (!isNaN(aNum) && !isNaN(bNum)) {
            return aNum - bNum;
        }
        
        // Fall back to string comparison
        return aValue.localeCompare(bValue);
    });
    
    // Reorder rows
    rows.forEach(row => tbody.appendChild(row));
}

/**
 * Handle global search
 */
function handleGlobalSearch(event) {
    const query = event.target.value.trim();
    if (query.length < 2) return;
    
    // This would implement global search across all content
    console.log('Global search for:', query);
}

/**
 * Get status icon for game status
 */
function getStatusIcon(status) {
    switch (status.toLowerCase()) {
        case 'complete': return '✅';
        case 'in progress': return '🔄';
        case 'planned': return '📋';
        case 'testing': return '🧪';
        default: return '❓';
    }
}

/**
 * Format timestamp for display
 */
function formatTime(timestamp) {
    const date = new Date(timestamp);
    const now = new Date();
    const diff = now - date;
    
    if (diff < 60000) return 'Just now';
    if (diff < 3600000) return `${Math.floor(diff / 60000)}m ago`;
    if (diff < 86400000) return `${Math.floor(diff / 3600000)}h ago`;
    return date.toLocaleDateString();
}

/**
 * Debounce function for search input
 */
function debounce(func, wait) {
    let timeout;
    return function executedFunction(...args) {
        const later = () => {
            clearTimeout(timeout);
            func(...args);
        };
        clearTimeout(timeout);
        timeout = setTimeout(later, wait);
    };
}

/**
 * Get sample game data for demonstration
 */
function getSampleGameData() {
    return [
        {
            name: 'Monster Hunter: World',
            engine: 'MT Framework',
            vrStatus: 'Complete',
            performance: '90+ FPS',
            lastTested: '2024-01-15'
        },
        {
            name: 'Resident Evil 7',
            engine: 'RE Engine',
            vrStatus: 'In Progress',
            performance: '75+ FPS',
            lastTested: '2024-01-14'
        },
        {
            name: 'Cyberpunk 2077',
            engine: 'REDengine 4',
            vrStatus: 'Planned',
            performance: 'TBD',
            lastTested: 'N/A'
        },
        {
            name: 'Fortnite',
            engine: 'Unreal Engine',
            vrStatus: 'Complete',
            performance: '95+ FPS',
            lastTested: '2024-01-13'
        },
        {
            name: 'Among Us',
            engine: 'Unity Engine',
            vrStatus: 'In Progress',
            performance: '80+ FPS',
            lastTested: '2024-01-12'
        }
    ];
}

/**
 * Get sample issues for demonstration
 */
function getSampleIssues() {
    return [
        {
            severity: 'Low',
            title: 'Minor visual artifact in RE Engine games',
            timestamp: new Date(Date.now() - 1800000).toISOString() // 30 minutes ago
        },
        {
            severity: 'Medium',
            title: 'Performance drop in open-world areas',
            timestamp: new Date(Date.now() - 3600000).toISOString() // 1 hour ago
        },
        {
            severity: 'High',
            title: 'Crash when switching VR modes',
            timestamp: new Date(Date.now() - 7200000).toISOString() // 2 hours ago
        }
    ];
}

/**
 * Error handling utility
 */
function handleError(error, context = 'Unknown operation') {
    console.error(`Error in ${context}:`, error);
    
    // Show user-friendly error message
    const errorMessage = document.createElement('div');
    errorMessage.className = 'error-message';
    errorMessage.innerHTML = `
        <div style="background: #f8d7da; color: #721c24; padding: 1rem; border-radius: 5px; margin: 1rem 0;">
            <strong>Error:</strong> ${error.message || 'An unexpected error occurred'}
        </div>
    `;
    
    // Insert at top of main content
    const main = document.querySelector('main');
    if (main) {
        main.insertBefore(errorMessage, main.firstChild);
        
        // Remove after 5 seconds
        setTimeout(() => {
            if (errorMessage.parentNode) {
                errorMessage.parentNode.removeChild(errorMessage);
            }
        }, 5000);
    }
}

/**
 * API utility functions
 */
const API = {
    async get(endpoint) {
        try {
            const response = await fetch(`${CONFIG.apiBaseUrl}${endpoint}`);
            if (!response.ok) {
                throw new Error(`HTTP ${response.status}: ${response.statusText}`);
            }
            return await response.json();
        } catch (error) {
            throw new Error(`API request failed: ${error.message}`);
        }
    },
    
    async post(endpoint, data) {
        try {
            const response = await fetch(`${CONFIG.apiBaseUrl}${endpoint}`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(data)
            });
            
            if (!response.ok) {
                throw new Error(`HTTP ${response.status}: ${response.statusText}`);
            }
            
            return await response.json();
        } catch (error) {
            throw new Error(`API request failed: ${error.message}`);
        }
    }
};

// Export for use in other scripts
window.uevrLCX = {
    API,
    gameData,
    performanceData,
    performSearch,
    updateGameTable,
    handleError
};
